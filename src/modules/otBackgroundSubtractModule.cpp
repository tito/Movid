#include <assert.h>
#include "otBackgroundSubtractModule.h"
#include "cv.h"

MODULE_DECLARE(BackgroundSubtract, "native", "subtracts the background from teh current input image.  stores next frame as background once when 'recapture is set to true.'");

otBackgroundSubtractModule::otBackgroundSubtractModule() : otImageFilterModule(){
	MODULE_INIT();
	
	// declare properties
	this->properties["recapture"] = new otProperty(true);
}

otBackgroundSubtractModule::~otBackgroundSubtractModule() {
}


void otBackgroundSubtractModule::allocateBuffers(){
	IplImage* src = (IplImage*)(this->input->getData());
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);	
	this->bg_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
	LOG(DEBUG) << "allocated output and background buffers for BackgroundSubtract module.";
}

void otBackgroundSubtractModule::applyFilter(){
	IplImage* src = (IplImage*)(this->input->getData());
	
	//check for recapture
	if (this->property("recapture").asBool()){
		cvCopy(src, this->bg_buffer);
		this->property("recapture").set(false);
		LOG(DEBUG) << "recaptured background in BackgroundSubtract module.";
	}

	//do subtraction
	cvSub(src, this->bg_buffer, this->output_buffer);
}


