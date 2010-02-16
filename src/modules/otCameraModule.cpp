#include <assert.h>

#include "../otLog.h"
#include "../otModule.h"
#include "../otDataStream.h"
#include "otCameraModule.h"
#include "highgui.h"

LOG_DECLARE("Camera");

otCameraModule::otCameraModule() : otModule(OT_MODULE_OUTPUT, 0, 1) {

	LOG(DEBUG) << "Create otCameraModule";

	this->camera = NULL;
	this->stream = new otDataStream("IplImage");

	// declare outputs
	this->output_names[0] = std::string("camera");
	this->output_types[0] = std::string("IplImage");

	// declare properties
	this->properties["index"] = new otProperty(0);
}

otCameraModule::~otCameraModule() {
	this->stop();
}

void otCameraModule::start() {
	assert( this->camera == NULL );

	otModule::start();

	// FIXME instead of use 0, use a property !
	LOG(INFO) << "Camera capture started";
	this->camera = cvCaptureFromCAM(this->property("index").asInteger());
}

void otCameraModule::stop() {
	if ( this->camera != NULL ) {
		LOG(DEBUG) << "Release camera capture";
		cvReleaseCapture((CvCapture **)&this->camera);
		this->camera = NULL;
	}
}

void otCameraModule::update() {
	// push a new image on the stream
	LOG(DEBUG) << "Push a new image on the stream";
	this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->camera)));
}

void otCameraModule::setInput(otDataStream* input, int n) {
	assert( "no input supported on otCameraModule()" && 0 );
}

otDataStream* otCameraModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

