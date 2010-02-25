#include <assert.h>

#include "../moLog.h"
#include "../moModule.h"
#include "../moDataStream.h"
#include "moImageModule.h"
#include "highgui.h"

MODULE_DECLARE(Image, "native", "Use a static image as source");

moImageModule::moImageModule() : moModule(MO_MODULE_OUTPUT, 0, 1) {

	MODULE_INIT();

	this->image = NULL;
	this->stream = new moDataStream("IplImage");

	// declare outputs
	this->output_infos[1] = new moDataStreamInfo(
			"image", "IplImage", "Image stream on a static image");

	// declare properties
	this->properties["filename"] = new moProperty("");
}

moImageModule::~moImageModule() {
}

void moImageModule::start() {
	assert( this->image == NULL );
	moModule::start();

	this->image = cvLoadImage(this->property("filename").asString().c_str());
	if ( this->image == NULL )
		LOGM(ERROR) << "could not load image: " << this->property("filename").asString();
}

void moImageModule::stop() {
	if ( this->image != NULL ) {
		LOGM(TRACE) << "release Image";
		// FIXME release !!!
		this->image = NULL;
	}
	moModule::stop();
}

void moImageModule::update() {
	if ( this->image != NULL ) {
		// push a new image on the stream
		LOGM(TRACE) << "push a new image on the stream";
		this->stream->push(this->image);
	}
}

void moImageModule::setInput(moDataStream* input, int n) {
	assert( "no input supported on moImageModule()" && 0 );
}

moDataStream* moImageModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

moDataStream* moImageModule::getInput(int n) {
	return NULL;
}

