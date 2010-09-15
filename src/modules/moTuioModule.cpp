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

enum {
	TUIO_UNKNOWN = 0,
	TUIO_2DCUR,
	TUIO_2DOBJ,
};

moTuioModule::moTuioModule() : moModule(MO_MODULE_INPUT) {

	MODULE_INIT();

	this->input = NULL;
	this->osc	= NULL;
	this->fseq	= 0;
	this->type	= TUIO_UNKNOWN;

	// declare inputs
	this->declareInput(0, &this->input, new moDataStreamInfo(
			"data", "trackedblob", "Data stream with type of 'trackedblob'"));

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
	moDataGenericList::iterator it;
	moDataGenericList *list;

	assert( input != NULL );
	assert( input == this->input );

	// out input have been updated !
	LOGM(MO_TRACE, "Updating Tuio stream");
	this->input->lock();

	list = static_cast<moDataGenericList *>(this->input->getData());

	// we must have data to probe the first time which data we got
	// and extract if it will be fiducial or simple blob
	if ( this->type == TUIO_UNKNOWN ) {
		it = list->begin();
		if ( it == list->end() ) {
			this->input->unlock();
			return;
		}

		std::string implements = (*it)->properties["implements"]->asString();
		if ( moUtils::inList("fiducial", implements) || moUtils::inList("markerlessobject", implements) )
			this->type = TUIO_2DOBJ;
		else
			this->type = TUIO_2DCUR;
	}

	std::string osc_path;
	if ( this->type == TUIO_2DOBJ )
		osc_path = "/tuio/2Dobj";
	else
		osc_path = "/tuio/2Dcur";

	//
	// Alive message
	//

	bundle = new WOscBundle();
	WOscMessage *msg = new WOscMessage(osc_path.c_str());
	msg->Add("alive");

	for ( it = list->begin(); it != list->end(); it++ ) {
		assert(moUtils::inList("tracked", (*it)->properties["implements"]->asString()));
		assert(moUtils::inList("pos", (*it)->properties["implements"]->asString()));
		msg->Add((*it)->properties["blob_id"]->asInteger());
	}

	bundle->Add(msg);

	//
	// Set message
	//

	for ( it = list->begin(); it != list->end(); it++ ) {
		msg = new WOscMessage(osc_path.c_str());
		msg->Add("set");
		if ( this->type == TUIO_2DOBJ ) {
			// /tuio/2Dobj set s i x y a X Y A m r
			msg->Add((*it)->properties["blob_id"]->asInteger()); // class id
			msg->Add((*it)->properties["fiducial_id"]->asInteger()); // class id
			msg->Add((float)(*it)->properties["x"]->asDouble()); // x
			msg->Add((float)(*it)->properties["y"]->asDouble()); // y
			msg->Add((float)(*it)->properties["angle"]->asDouble()); // a
			msg->Add((float)0.); // X
			msg->Add((float)0.); // Y
			msg->Add((float)0.); // A
			msg->Add((float)0.); // m
			msg->Add((float)0.); // r
		} else if ( this->type == TUIO_2DCUR ) {
			// /tuio/2Dcur set s x y X Y m
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
		}
		bundle->Add(msg);
	}

	//
	// Frame sequence message
	//

	msg = new WOscMessage(osc_path.c_str());
	msg->Add("fseq");
	msg->Add(this->fseq++);
	bundle->Add(msg);

    LOGM(MO_TRACE, "Sending OSC bundle");
	this->osc->send(bundle);

	delete bundle;

	this->input->unlock();
	LOGM(MO_TRACE, "TUIO done");
}

void moTuioModule::update() {
}


