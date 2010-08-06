/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/

/*
 * TUIO 2 Module
 *
 * Specifications : http://tuio.org/?tuio20
 *
 * Currently implemented : FRM, ALV, PTR, ALA, COA, TOK, BND(for PTR)
 *
 * Todo : LIA, SYM, T3D, P3D, B3D
 * + support multiple input (sync frame problem)
 * + check implements
 *
 *
 * Needed from the previous module:
 * - blob_id (from tracked implements)
 * - parent_node_id (from node implements)
 * - x (from pos implements)
 * - y (from pos implements)
 */


#include <sstream>
#include <list>
#include <assert.h>

#include "moTuio2Module.h"
#include "../moLog.h"
#include "../moDataGenericContainer.h"
#include "../moDataStream.h"
#include "../moOSC.h"
#include "../moUtils.h"

MODULE_DECLARE(Tuio2, "native", "Convert stream to TUIO2 format");

moTuio2Module::moTuio2Module() : moModule(MO_MODULE_INPUT, 1, 0) {

	MODULE_INIT();

	this->input = NULL;
	this->osc	= NULL;
	this->frame	= 0;

	// declare inputs
	this->input_infos[0] = new moDataStreamInfo(
			"data", "moDataGenericList", "Data stream with type of 'blob' or 'fiducial'");

	// declare properties
	this->properties["ip"] = new moProperty("127.0.0.1");
	this->properties["port"] = new moProperty(3333);
}

moTuio2Module::~moTuio2Module(){
	if ( this->osc != NULL )
		delete this->osc;
}

void moTuio2Module::start() {
	this->osc = new moOSC(
		this->property("ip").asString(),
		this->property("port").asInteger()
	);
	moModule::start();
}

void moTuio2Module::stop() {
	moModule::stop();
	delete this->osc;
	this->osc = NULL;
}

void moTuio2Module::notifyData(moDataStream *input) {
	moDataGenericList::iterator it;
	moDataGenericList *list;
	WOscBundle *bundle;
	WOscMessage *msg;
	std::string format, implements;
	bool ret = false,
		 have_relation = false;
	std::list<int> ids_parent, ids_blob;
	std::list<int>::iterator it_parent, it_blob;
	std::map<int,int> map_node_blob;
	int id_blob, id_parent;

	assert( input != NULL );
	assert( input == this->input );

	// lock input to ensure data will be not be modified
	this->input->lock();

	// create OSC bundle
	bundle = new WOscBundle();

	// append frame message
	msg = new WOscMessage("/tuio2/frm");
	msg->Add(this->frame++);
	msg->Add((int)moUtils::time());
	msg->Add("movid"); // FIXME add a config.h, and use this value
	bundle->Add(msg);

	// append alive blob/object/...
	msg = new WOscMessage("/tuio2/alv");
	list = (moDataGenericList *)this->input->getData();
	for ( it = list->begin(); it != list->end(); it++ ) {

		implements = (*it)->properties["implements"]->asString();

		// ensure the item is tracked
		if ( !moUtils::inList("tracked", implements) ) {
			// FIXME launch error, we have received a item without tracked implementation !
			continue;
		}

		// item is tracked, we can safely use blob_id
		id_blob = (*it)->properties["blob_id"]->asInteger();
		msg->Add(id_blob);

		// use this loop to determine if we must send link relation

		// item have tree information, use it !
		if ( moUtils::inList("node", implements) ) {
			map_node_blob[(*it)->properties["node_id"]->asInteger()] = id_blob;
			// since parent_node_id is not mandatory, check first.
			if ( (*it)->exist("parent_node_id") ) {
				have_relation = true;
				ids_blob.push_back(id_blob);
				ids_parent.push_back((*it)->properties["parent_node_id"]->asInteger());
			}
		}
	}
	bundle->Add(msg);

	// pack every item into the bundle
	// (format have already been checked at setInput()
	format = input->getFormat();
	if ( format == "fiducial" ) {
		ret = this->packFiducial(bundle);
	} else if ( format == "blob" ) {
		ret = this->packBlob(bundle);
	}

	// if relation have been found between blob, just add them
	if ( have_relation == true ) {
		// ensure parent / blob are not duplicate
		ids_parent.unique();
		ids_blob.unique();

		// send alive message only for item who have relation
		msg = new WOscMessage("/tuio2/ala");
		for ( it_parent = ids_parent.begin(); it_parent != ids_parent.end(); it_parent++ )
			msg->Add(map_node_blob[*it_parent]);
		for ( it_blob = ids_blob.begin(); it_blob != ids_blob.end(); it_blob++ )
			msg->Add(*it_blob);
		bundle->Add(msg);

		// now send relation with coa message
		for ( it_parent = ids_parent.begin(); it_parent != ids_parent.end(); it_parent++ ) {

			id_parent = map_node_blob[*it_parent];

			// create the coa message, and add the parent blob_id
			msg = new WOscMessage("/tuio2/coa");
			msg->Add(id_parent);
			msg->Add(0); // FIXME slot

			// now add all children of this current parent
			for ( it = list->begin(); it != list->end(); it++ ) {
				implements = (*it)->properties["implements"]->asString();
				if ( !moUtils::inList("node", implements) )
					continue;
				if ( !(*it)->exist("parent_node_id") )
					continue;
				id_parent = (*it)->properties["parent_node_id"]->asInteger();
				if ( id_parent != *it_parent )
					continue;
				msg->Add((*it)->properties["blob_id"]->asInteger());
			}

			bundle->Add(msg);
		}
		
	}

	// unlock input before sending (no more input usage)
	this->input->unlock();

	// send only if packing is done correctly
	if ( ret )
		this->osc->send(bundle);

	delete bundle;
}

bool moTuio2Module::packBlob(WOscBundle *bundle) {
	moDataGenericList::iterator it;
	moDataGenericList *list;
	WOscMessage *msg;

	list = (moDataGenericList *)this->input->getData();
	for ( it = list->begin(); it != list->end(); it++ ) {
		// format is /tuio2/ptr s_id tu_id c_id x_pos y_pos width press [x_vel y_vel m_acc] 
		msg = new WOscMessage("/tuio2/ptr");
		msg->Add((*it)->properties["blob_id"]->asInteger());	// s_id
		msg->Add(1);											// tu_id (default right index)
		msg->Add((*it)->properties["blob_id"]->asInteger());	// c_id
		msg->Add((float)(*it)->properties["x"]->asDouble());	// x_pos
		msg->Add((float)(*it)->properties["y"]->asDouble());	// y_pos
		msg->Add(0);											// width
		msg->Add(1);											// press (1=pressed)
		bundle->Add(msg);

		if ( moUtils::inList("size", (*it)->properties["implements"]->asString()) ) {
			msg = new WOscMessage("/tuio2/bnd");
			msg->Add((*it)->properties["blob_id"]->asInteger());	// s_id
			msg->Add((float)(*it)->properties["x"]->asDouble());	// x_pos
			msg->Add((float)(*it)->properties["y"]->asDouble());	// y_pos
			msg->Add((float)0);										// angle
			msg->Add((float)(*it)->properties["width"]->asDouble());	// width
			msg->Add((float)(*it)->properties["height"]->asDouble());	// height
			msg->Add((float)1.0);									// area (FIXME)
			bundle->Add(msg);
		}
	}

	return true;
}

bool moTuio2Module::packFiducial(WOscBundle *bundle) {
	moDataGenericList::iterator it;
	moDataGenericList *list;
	WOscMessage *msg;

	list = (moDataGenericList *)this->input->getData();
	for ( it = list->begin(); it != list->end(); it++ ) {
		// format is /tuio2/tok s_id tu_id c_id x_pos y_pos angle [x_vel y_vel a_vel m_acc r_acc]
		msg = new WOscMessage("/tuio2/tok");
		msg->Add((*it)->properties["blob_id"]->asInteger());	// s_id
		msg->Add((*it)->properties["blob_id"]->asInteger());	// tu_id
		msg->Add((*it)->properties["blob_id"]->asInteger());	// c_id
		msg->Add((float)(*it)->properties["x"]->asDouble());	// x_pos
		msg->Add((float)(*it)->properties["y"]->asDouble());	// y_pos
		msg->Add((float)(*it)->properties["angle"]->asDouble());// y_pos
		bundle->Add(msg);
	}

	return true;
}

void moTuio2Module::setInput(moDataStream *stream, int n) {
	std::string format;
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return;
	}
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( stream != NULL ) {
		format = stream->getFormat();
		if ( format != "blob" && format != "fiducial" ) {
			this->setError("Input 0 only accepts blobs or fiducial, but got " + stream->getFormat());
			this->input = NULL;
			return;
		}
	}
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moTuio2Module::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moTuio2Module::getOutput(int n) {
	this->setError("no output supported");
	return NULL;
}

void moTuio2Module::update() {
}

