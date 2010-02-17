#include <assert.h>
#include "otThresholdModule.h"
#include "cv.h"

MODULE_DECLARE(Threshold, "native", "Thresholding to throw away all values below or above certain threshold");

otThresholdModule::otThresholdModule() : otImageFilterModule(){
	MODULE_INIT();
	
	// declare properties
	this->properties["threshold"] = new otProperty(50);
	this->properties["adaptive"] = new otProperty(false);
	this->properties["block_size"] = new otProperty(21); // size fo neighbor hood to compare to for adaptive threshold
}

otThresholdModule::~otThresholdModule() {
}



void otThresholdModule::applyFilter(){
	IplImage* src = (IplImage*)this->input->getData();
	assert( "threshold filter needs single channel input" && (src->nChannels == 1) );

	if (this->property("adaptive").asBool()){
		cvAdaptiveThreshold( (IplImage*)this->input->getData(),
					this->output_buffer,
					255.0, //max value is output of where threshold was passed
					CV_ADAPTIVE_THRESH_MEAN_C,
					CV_THRESH_BINARY,
					this->property("block_size").asInteger(),
					this->property("threshold").asDouble()*-1 //other way around on adpative, pass if src > (AVRG(block) - this arg)...so pixel pass if brighter than average neighboorhood + thresh (-1* -thresh)
					);
	}
	else{
		cvThreshold( (IplImage*)this->input->getData(),
					this->output_buffer,
					this->property("threshold").asDouble(),
					255.0, //max value is output of where threshold was passed
					CV_THRESH_BINARY
					);
	}

}


