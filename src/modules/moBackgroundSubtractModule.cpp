#include <assert.h>
#include "moBackgroundSubtractModule.h"
#include "cv.h"

MODULE_DECLARE(BackgroundSubtract, "native", "subtracts the background from teh current input image.  stores next frame as background once when 'recapture is set to true.'");

moBackgroundSubtractModule::moBackgroundSubtractModule() : moImageFilterModule(){
	MODULE_INIT();

	// declare properties
	this->properties["recapture"] = new moProperty(true);
}

moBackgroundSubtractModule::~moBackgroundSubtractModule() {
}

void moBackgroundSubtractModule::stop() {
	if ( this->output_buffer != NULL ) {
		cvReleaseImage(&this->output_buffer);
		this->output_buffer = NULL;
	}
	if ( this->bg_buffer != NULL ) {
		cvReleaseImage(&this->bg_buffer);
		this->bg_buffer = NULL;
	}
	this->need_update = false;
	this->property("recapture").set(true);

	moImageFilterModule::stop();
}

void moBackgroundSubtractModule::allocateBuffers() {
	IplImage* src = (IplImage*)(this->input->getData());
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
	this->bg_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
	LOG(DEBUG) << "allocated output and background buffers for BackgroundSubtract module.";
}

void moBackgroundSubtractModule::applyFilter() {
	assert( this->bg_buffer != NULL );
	assert( this->output_buffer != NULL );

	IplImage* src = (IplImage*)(this->input->getData());
	if ( src == NULL )
		return;

	// check for recapture
	if (this->property("recapture").asBool()) {
		cvCopy(src, this->bg_buffer);
		this->property("recapture").set(false);
		LOG(DEBUG) << "recaptured background in BackgroundSubtract module.";
	}

	// do subtraction
	cvSub(src, this->bg_buffer, this->output_buffer);
}

