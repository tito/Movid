#include <assert.h>

#include "../otLog.h"
#include "../otModule.h"
#include "../otDataStream.h"
#include "otImageModule.h"
#include "highgui.h"

MODULE_DECLARE(Image, "native", "Use a static image as source");

otImageModule::otImageModule() : otModule(OT_MODULE_OUTPUT, 0, 1) {

	MODULE_INIT();

	this->image = NULL;
	this->stream = new otDataStream("IplImage");

	// declare outputs
	this->output_infos[1] = new otDataStreamInfo(
			"image", "IplImage", "Image stream on a static image");

	// declare properties
	this->properties["filename"] = new otProperty("");
}

otImageModule::~otImageModule() {
}

void otImageModule::start() {
	assert( this->image == NULL );
	otModule::start();

	this->image = cvLoadImage(this->property("filename").asString().c_str());
	if ( this->image == NULL )
		LOGM(ERROR) << "could not load image: " << this->property("filename").asString();
}

void otImageModule::stop() {
	if ( this->image != NULL ) {
		LOGM(TRACE) << "release Image";
		// FIXME release !!!
		this->image = NULL;
	}
	otModule::stop();
}

void otImageModule::update() {
	if ( this->image != NULL ) {
		// push a new image on the stream
		LOGM(TRACE) << "push a new image on the stream";
		this->stream->push(this->image);
	}
}

void otImageModule::setInput(otDataStream* input, int n) {
	assert( "no input supported on otImageModule()" && 0 );
}

otDataStream* otImageModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

otDataStream* otImageModule::getInput(int n) {
	return NULL;
}

