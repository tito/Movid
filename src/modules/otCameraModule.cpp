#include <assert.h>

#include "../otLog.h"
#include "../otModule.h"
#include "../otDataStream.h"
#include "otCameraModule.h"
#include "highgui.h"

MODULE_DECLARE(Camera, "native", "Fetch camera stream");

otCameraModule::otCameraModule() : otModule(OT_MODULE_OUTPUT, 0, 1) {

	MODULE_INIT();

	this->camera = NULL;
	this->stream = new otDataStream("IplImage");

	// declare outputs
	this->output_names[0] = std::string("camera");
	this->output_types[0] = std::string("IplImage");

	// declare properties
	this->properties["index"] = new otProperty(0);
}

otCameraModule::~otCameraModule() {
}

void otCameraModule::start() {
	assert( this->camera == NULL );
	otModule::start();
	LOGM(TRACE) << "start camera";
	if(!(this->camera = cvCaptureFromCAM(this->property("index").asInteger()))) {
		this->camera = NULL;
		LOGM(ERROR) << "could not load camera: " << this->property("index").asInteger();
	}
}

void otCameraModule::stop() {
	if ( this->camera != NULL ) {
		LOGM(TRACE) << "release camera";
		cvReleaseCapture((CvCapture **)&this->camera);
		this->camera = NULL;
	}
	otModule::stop();
}

void otCameraModule::update() {
	if ( this->camera != NULL ) {
		// push a new image on the stream
		LOGM(TRACE) << "push a new image on the stream";
		this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->camera)));
	}
}

void otCameraModule::setInput(otDataStream* input, int n) {
	assert( "no input supported on otCameraModule()" && 0 );
}

otDataStream* otCameraModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

otDataStream* otCameraModule::getInput(int n) {
	return NULL;
}

