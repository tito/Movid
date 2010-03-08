#include <assert.h>
#include "moGrayScaleModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(GrayScale, "native", "Converts input image to a one bit channel image");

moGrayScaleModule::moGrayScaleModule() : moImageFilterModule(){
	MODULE_INIT();
}

moGrayScaleModule::~moGrayScaleModule() {
}

void moGrayScaleModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, 1);	//only one channel
	LOG(MO_DEBUG) << "allocated output buffer for GrayScale module.";
}

void moGrayScaleModule::applyFilter() {
	IplImage* src = (IplImage*)(this->input->getData());
	cvCvtColor(src, this->output_buffer, CV_RGB2GRAY);
}

