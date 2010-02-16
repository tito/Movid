#include "otGaussianBlurModule.h"
#include "cv.h"


otGaussianBlurModule::otGaussianBlurModule() : otImageFilterModule(){
	// declare properties
	this->properties["width"] = new otProperty(5.);
	this->properties["height"] = new otProperty(5.);
}


void otGaussianBlurModule::applyFilter(){
	cvSmooth(
			 (IplImage*)this->input->getData(),
			 this->output_buffer, 
			 CV_GAUSSIAN,
			 this->property("width").asDouble(),
			 this->property("height").asDouble()
			);
}


