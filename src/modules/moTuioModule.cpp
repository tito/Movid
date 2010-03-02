#include <sstream>
#include <assert.h>

#include "moTuioModule.h"
#include "../moDataGenericContainer.h"
#include "../moDataStream.h"
#include "../moOSC.h"

MODULE_DECLARE(Tuio, "native", "Convert stream to TUIO format (touch & objects)");

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
	// ensure that input data is IfiImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "moDataGenericList" );

	// out input have been updated !
	this->input->lock();

	WOscBundle	*bundle = new WOscBundle();

	// TODO OPTIMIZE PROCESS !!!
	//
	// PHASE 1: objects
	//

	WOscMessage *msg = new WOscMessage("/tuio/2Dobj");
	msg->Add("alive");

	moDataGenericList::iterator it;
	moDataGenericList *list = (moDataGenericList *)this->input->getData();
	for ( it = list->begin(); it != list->end(); it++ ) {
		if ( (*it)->properties["type"]->asString() != "fiducial" )
			continue;
		msg->Add((*it)->properties["id"]->asString().c_str());
	}

	bundle->Add(msg);

	for ( it = list->begin(); it != list->end(); it++ ) {
		if ( (*it)->properties["type"]->asString() != "fiducial" )
			continue;
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

	//
	// PHASE 2: touches
	//
	// TODO

	this->osc->send(bundle);

	delete bundle;

	this->input->unlock();
}

void moTuioModule::setInput(moDataStream *input, int n) {
	assert( n == 0 );
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = input;
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moTuioModule::getInput(int n) {
	assert( n == 0);
	return this->input;
}

moDataStream* moTuioModule::getOutput(int n) {
	assert( "moTuioModule don't accept output" && 0 );
	return NULL;
}

void moTuioModule::update() {
}


