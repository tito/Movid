#include <assert.h>
#include "otModule.h"
#include "otDataStream.h"
#include "otCameraModule.h"
#include "highgui.h"

otCameraModule::otCameraModule() : otModule(OT_MODULE_OUTPUT, 0, 1) {
	this->camera = NULL;
	this->stream = new otDataStream("IplImage");
}

otCameraModule::~otCameraModule() {
	this->stop();
}

void otCameraModule::start() {
	otModule::start();

	assert( this->camera == NULL );

	// FIXME instead of use 0, use a property !
	this->camera = cvCaptureFromCAM(0);
}

void otCameraModule::stop() {
	if ( this->camera != NULL ) {
		cvReleaseCapture((CvCapture **)&this->camera);
		this->camera = NULL;
	}
}

void otCameraModule::update() {
	// push a new image on the stream
	this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->camera)));
}

void otCameraModule::setInput(otDataStream* input, int n) {
	assert( "no input supported on otCameraModule()" && 0 );
}

otDataStream* otCameraModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

std::string otCameraModule::getOutputName(int n) {
	assert( n == 0 );
	return "camera";
}

std::string otCameraModule::getOutputType(int n) {
	assert( n == 0 );
	return "IplImage";
}
