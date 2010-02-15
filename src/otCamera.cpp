#include <assert.h>
#include "otModule.h"
#include "otDataStream.h"
#include "otCamera.h"
#include "highgui.h"

otCamera::otCamera() : otModule(OT_MODULE_OUTPUT, 0, 1) {
	this->camera = NULL;
	this->stream = new otDataStream("IplImage");
}

otCamera::~otCamera() {
	this->stop();
}

void otCamera::start() {
	otModule::start();

	assert( this->camera == NULL );

	// FIXME instead of use 0, use a property !
	this->camera = cvCaptureFromCAM(0);
}

void otCamera::stop() {
	if ( this->camera != NULL ) {
		cvReleaseCapture((CvCapture **)&this->camera);
		this->camera = NULL;
	}
}

void otCamera::update() {
	// push a new image on the stream
	this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->camera)));
}

void otCamera::setInput(otDataStream* input, int n) {
	assert( "no input supported on otCamera()" && 0 );
}

otDataStream* otCamera::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

