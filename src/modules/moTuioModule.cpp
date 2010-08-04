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


//
// TUIO Module, supporting GenericTouch and GenericFiducial 
//
// Specifications : http://tuio.org/?specification
//
// Currently implemented :
// 2D Interactive Surface
//
//  /tuio/2Dobj set s i x y a X Y A m r
//  /tuio/2Dcur set s x y X Y m
//
// Table 1: semantic types of set messages
// s 		Session ID (temporary object ID) 	int32
// i 		Class ID (e.g. marker ID) 			int32
// x, y, z 	Position 							float32, range 0...1
// a, b, c 	Angle 								float32, range 0..2PI
// w, h, d 	Dimension 							float32, range 0..1
// f, v 	Area, Volume 						float32, range 0..1
// X, Y ,Z 	Velocity vector (motion speed & direction) 	float32
// A, B, C 	Rotation velocity vector (rotation speed & direction) 	float32
// m 		Motion acceleration 				float32
// r 		Rotation acceleration 				float32
// P 		Free parameter 						type defined by OSC message header



#include <sstream>
#include <assert.h>

#include "moTuioModule.h"
#include "../moLog.h"
#include "../moDataGenericContainer.h"
#include "../moDataStream.h"
#include "../moOSC.h"
#include "../moUtils.h"

MODULE_DECLARE(Tuio, "native", "Convert stream to TUIO format (touch & fiducial)");

moTuioModule::moTuioModule() : moModule(MO_MODULE_INPUT, 1, 0) {

	MODULE_INIT();

	this->input = NULL;
	this->osc	= NULL;
	this->fseq	= 0;

	// declare inputs
	this->input_infos[0] = new moDataStreamInfo(
			"data", "moDataGenericList", "Data stream with type of 'blob' or 'fiducial'");

	// declare properties
	this->properties["ip"] = new moProperty("127.0.0.1");
	this->properties["port"] = new moProperty(3333);
	this->properties["sendsize"] = new moProperty(true);
}

moTuioModule::~moTuioModule(){
	if ( this->osc != NULL )
		delete this->osc;
}

void moTuioModule::start() {
	this->osc = new moOSC(
		this->property("ip").asString(),
		this->property("port").asInteger()
	);
	moModule::start();
}

void moTuioModule::stop() {
	moModule::stop();
	delete this->osc;
	this->osc = NULL;
}

void moTuioModule::notifyData(moDataStream *input) {
	WOscBundle *bundle = NULL;

	assert( input != NULL );
	assert( input == this->input );

	// out input have been updated !
	this->input->lock();

	// TODO also adapt fiducial tracker to new blob protocol
	if ( input->getFormat() == "fiducial" ) {

		bundle = new WOscBundle();
		WOscMessage *msg = new WOscMessage("/tuio/2Dobj");
		msg->Add("alive");

		moDataGenericList::iterator it;
		moDataGenericList *list = (moDataGenericList *)this->input->getData();
		for ( it = list->begin(); it != list->end(); it++ ) {
			assert(moUtils::inList("fiducial", (*it)->properties["implements"]->asString()));
			assert(moUtils::inList("pos", (*it)->properties["implements"]->asString()));
			assert(moUtils::inList("tracked", (*it)->properties["implements"]->asString()));
			msg->Add(atoi((*it)->properties["id"]->asString().c_str()));
		}

		bundle->Add(msg);

		for ( it = list->begin(); it != list->end(); it++ ) {
			msg = new WOscMessage("/tuio/2Dobj");
			msg->Add("set");
			msg->Add(9843); // session id
			msg->Add((*it)->properties["blob_id"]->asInteger()); // class id
			msg->Add((float)(*it)->properties["x"]->asDouble()); // x
			msg->Add((float)(*it)->properties["y"]->asDouble()); // y
			msg->Add((float)(*it)->properties["angle"]->asDouble()); // a
			msg->Add((float)0.); // X
			msg->Add((float)0.); // Y
			msg->Add((float)0.); // A
			msg->Add((float)0.); // m
			msg->Add((float)0.); // r
			bundle->Add(msg);
		}

		msg = new WOscMessage("/tuio/2Dobj");
		msg->Add("fseq");
		msg->Add(this->fseq++);
		bundle->Add(msg);


	} else if ( input->getFormat() == "blob" ) {
		// /tuio/2Dcur set s x y X Y m

		bundle = new WOscBundle();
		WOscMessage *msg = new WOscMessage("/tuio/2Dcur");
		msg->Add("alive");

		moDataGenericList::iterator it;
		moDataGenericList *list = (moDataGenericList *)this->input->getData();
		for ( it = list->begin(); it != list->end(); it++ ) {
			assert(moUtils::inList("tracked", (*it)->properties["implements"]->asString()));
			assert(moUtils::inList("pos", (*it)->properties["implements"]->asString()));
			// XXX Add an assert that checks if the blob implements x/y
			msg->Add((*it)->properties["blob_id"]->asInteger());
		}

		bundle->Add(msg);

		for ( it = list->begin(); it != list->end(); it++ ) {
			msg = new WOscMessage("/tuio/2Dcur");
			msg->Add("set");
			msg->Add((*it)->properties["blob_id"]->asInteger()); // class id
			msg->Add((float)(*it)->properties["x"]->asDouble()); // x
			msg->Add((float)(*it)->properties["y"]->asDouble()); // y
			msg->Add((float)0.); // X
			msg->Add((float)0.); // Y
			msg->Add((float)0.); // m
			if ( this->property("sendsize").asBool()
				&& moUtils::inList("size", (*it)->properties["implements"]->asString())
			) {
				msg->Add((float)(*it)->properties["width"]->asDouble()); // w
				msg->Add((float)(*it)->properties["height"]->asDouble()); // h
			}
			bundle->Add(msg);
		}

		msg = new WOscMessage("/tuio/2Dcur");
		msg->Add("fseq");
		msg->Add(this->fseq++);
		bundle->Add(msg);

	} else {
		LOGM(MO_ERROR, "Unsupported input type: " << input->getFormat());
		this->setError("Unsupported input type");
	}

	this->osc->send(bundle);

	delete bundle;

	this->input->unlock();
}

void moTuioModule::setInput(moDataStream *stream, int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return;
	}
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( stream != NULL ) {
		if ( stream->getFormat() != "blob" &&
			 stream->getFormat() != "fiducial" ) {
			this->setError("Input 0 only accepts blob or fiducial, but got " + stream->getFormat());
			this->input = NULL;
			return;
		}
	}
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moTuioModule::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moTuioModule::getOutput(int n) {
	this->setError("no output supported");
	return NULL;
}

void moTuioModule::update() {
}


