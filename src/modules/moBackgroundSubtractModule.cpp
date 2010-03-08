#include <assert.h>
#include "moBackgroundSubtractModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(BackgroundSubtract, "native",
	"Subtracts the background from the current input image.\n" \
	"Stores next frame as background once when 'recapture is set to true.'");

moBackgroundSubtractModule::moBackgroundSubtractModule() : moImageFilterModule() {

	MODULE_INIT();

	// declare properties
	this->properties["recapture"] = new moProperty(true);
}

moBackgroundSubtractModule::~moBackgroundSubtractModule() {
}

void moBackgroundSubtractModule::stop() {
	moImageFilterModule::stop();

	if ( this->output_buffer != NULL ) {
		cvReleaseImage(&this->output_buffer);
		this->output_buffer = NULL;
	}
	if ( this->bg_buffer != NULL ) {
		cvReleaseImage(&this->bg_buffer);
		this->bg_buffer = NULL;
	}

	// reset state
	this->property("recapture").set(true);
}

void moBackgroundSubtractModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
	this->bg_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
	LOGM(MO_TRACE) << "allocated output and background buffers";
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
		LOGM(MO_TRACE) << "recaptured background";
	}

	// do subtraction
	cvSub(src, this->bg_buffer, this->output_buffer);
}

