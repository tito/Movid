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

MODULE_DECLARE(Tuio, "native", "Convert stream to TUIO format (touch & fiducial)");

moTuioModule::moTuioModule() : moModule(MO_MODULE_INPUT, 1, 0) {

	MODULE_INIT();

	this->input = NULL;
	this->osc	= NULL;
	this->fseq	= 0;

	// declare inputs
	this->input_infos[0] = new moDataStreamInfo(
			"data", "moDataGenericList", "Data stream with type of 'touch' or 'fiducial'");

	// declare properties
	this->properties["ip"] = new moProperty("127.0.0.1");
	this->properties["port"] = new moProperty(3333);
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
}

void moTuioModule::stop() {
	delete this->osc;
	this->osc = NULL;
}

void moTuioModule::notifyData(moDataStream *input) {
	WOscBundle	*bundle = NULL;

	assert( input != NULL );
	assert( input == this->input );

	// out input have been updated !
	this->input->lock();

	if ( input->getFormat() == "GenericFiducial" ) {

		bundle = new WOscBundle();
		WOscMessage *msg = new WOscMessage("/tuio/2Dobj");
		msg->Add("alive");

		moDataGenericList::iterator it;
		moDataGenericList *list = (moDataGenericList *)this->input->getData();
		for ( it = list->begin(); it != list->end(); it++ ) {
			assert((*it)->properties["type"]->asString() == "fiducial");
			msg->Add((*it)->properties["id"]->asString().c_str());
		}

		bundle->Add(msg);

		for ( it = list->begin(); it != list->end(); it++ ) {
			assert((*it)->properties["type"]->asString() == "fiducial");
			msg = new WOscMessage("/tuio/2Dobj");
			msg->Add("set");
			msg->Add(9843); // session id
			msg->Add((*it)->properties["id"]->asInteger()); // class id
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


	} else if ( input->getFormat() == "GenericTouch" ) {
		// /tuio/2Dcur set s x y X Y m


		bundle = new WOscBundle();
		WOscMessage *msg = new WOscMessage("/tuio/2Dcur");
		msg->Add("alive");

		moDataGenericList::iterator it;
		moDataGenericList *list = (moDataGenericList *)this->input->getData();
		for ( it = list->begin(); it != list->end(); it++ ) {
			assert((*it)->properties["type"]->asString() == "touch");
			msg->Add((*it)->properties["id"]->asString().c_str());
		}

		bundle->Add(msg);

		for ( it = list->begin(); it != list->end(); it++ ) {
			assert((*it)->properties["type"]->asString() == "touch");

			msg = new WOscMessage("/tuio/2Dcur");
			msg->Add("set");
			msg->Add((*it)->properties["id"]->asInteger()); // class id
			msg->Add((float)(*it)->properties["x"]->asDouble()); // x
			msg->Add((float)(*it)->properties["y"]->asDouble()); // y
			msg->Add((float)0.); // X
			msg->Add((float)0.); // Y
			msg->Add((float)0.); // m
			msg->Add((float)(*it)->properties["w"]->asDouble()); // w
			msg->Add((float)(*it)->properties["h"]->asDouble()); // h
			bundle->Add(msg);
		}

		msg = new WOscMessage("/tuio/2Dcur");
		msg->Add("fseq");
		msg->Add(this->fseq++);
		bundle->Add(msg);

	} else {
		LOGM(MO_ERROR) << "Unsupported input type: " << input->getFormat();
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
		if ( stream->getFormat() != "GenericTouch" &&
			 stream->getFormat() != "GenericFiducial" ) {
			this->setError("Input 0 accept only touch or fiducial");
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


