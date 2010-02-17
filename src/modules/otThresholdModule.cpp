#include <assert.h>
#include "otThresholdModule.h"
#include "cv.h"

MODULE_DECLARE(Threshold, "native", "Thresholding to throw away all values below or above certain threshold");

otThresholdModule::otThresholdModule() : otImageFilterModule(){
	MODULE_INIT();
	
	// declare properties
	this->properties["threshold"] = new otProperty(100.0);
	this->properties["type"] = new otProperty("binary");
}

otThresholdModule::~otThresholdModule() {
}


static int cv_thresh_type(std::string type){
	if ( type == "binary" )
		return CV_THRESH_BINARY;
	if ( type == "truncate" )
		return CV_THRESH_TRUNC;
	assert( "unimplemented filter" && 0 );
}


void otThresholdModule::applyFilter(){
	IplImage* src = (IplImage*)this->input->getData();
	assert( "threshold filter needs single channel input" && (src->nChannels == 1) );
	
	cvThreshold( (IplImage*)this->input->getData(),
				this->output_buffer,
			    this->property("threshold").asDouble(),
			    255.0, //max value is output of where threshold was passed
			    cv_thresh_type(this->property("type").asString())
			  );
}


