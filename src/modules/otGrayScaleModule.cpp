#include <assert.h>
#include "otGrayScaleModule.h"
#include "cv.h"

MODULE_DECLARE(GrayScale, "native", "Converts input image to GrayScale");

otGrayScaleModule::otGrayScaleModule() : otImageFilterModule(){
	MODULE_INIT();
}

otGrayScaleModule::~otGrayScaleModule() {
}

void otGrayScaleModule::allocateBuffers() {
	IplImage* src = (IplImage*)(this->input->getData());
	if ( src == NULL )
		return;
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, 1);	//only one channel
	LOG(DEBUG) << "allocated output buffer for GrayScale module.";
}

void otGrayScaleModule::applyFilter() {
	IplImage* src = (IplImage*)(this->input->getData());
	cvCvtColor( src, this->output_buffer, CV_RGB2GRAY );
}

