#include <assert.h>

#include "../moLog.h"
#include "../moModule.h"
#include "../moDataStream.h"
#include "moCameraModule.h"
#include "highgui.h"

MODULE_DECLARE(Camera, "native", "Fetch camera stream");

moCameraModule::moCameraModule() : moModule(MO_MODULE_OUTPUT, 0, 1) {

	MODULE_INIT();

	this->camera = NULL;
	this->stream = new moDataStream("IplImage");

	// declare outputs
	this->output_infos[0] = new moDataStreamInfo(
			"camera", "IplImage", "Image stream of the camera");

	// declare properties
	this->properties["index"] = new moProperty(0);
}

moCameraModule::~moCameraModule() {
}

void moCameraModule::start() {
	assert( this->camera == NULL );
	moModule::start();
	LOGM(TRACE) << "start camera";

	this->camera = cvCaptureFromCAM(this->property("index").asInteger());
	if ( this->camera == NULL ) {
		LOGM(ERROR) << "could not load camera: " << this->property("index").asInteger();
		this->setError("Unable to open camera");
	}
}

void moCameraModule::stop() {
	if ( this->camera != NULL ) {
		LOGM(TRACE) << "release camera";
		cvReleaseCapture((CvCapture **)&this->camera);
		this->camera = NULL;
	}
	moModule::stop();
}

void moCameraModule::update() {
	if ( this->camera != NULL ) {
		// push a new image on the stream
		LOGM(TRACE) << "push a new image on the stream";
		this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->camera)));
		this->notifyUpdate();
	}
}

void moCameraModule::poll() {
	this->notifyUpdate();
	moModule::poll();
}

void moCameraModule::setInput(moDataStream* input, int n) {
	this->setError("no input supported");
}

moDataStream* moCameraModule::getOutput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid output index");
		return NULL;
	}
	return this->stream;
}

moDataStream* moCameraModule::getInput(int n) {
	return NULL;
}

