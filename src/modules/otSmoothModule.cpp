#include <assert.h>
#include "otSmoothModule.h"
#include "cv.h"

MODULE_DECLARE(Smooth, "native", "Smooth an image with one of several filters");

otSmoothModule::otSmoothModule() : otImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["size"] = new otProperty(1.);
	this->properties["filter"] = new otProperty("gaussian");
}

otSmoothModule::~otSmoothModule() {
}

static int cv_smooth_type(std::string filter){
	if ( filter == "median" )
		return CV_MEDIAN;
	if ( filter == "gaussian" )
		return CV_GAUSSIAN;
	if ( filter == "blur" )
		return CV_BLUR;
	if ( filter == "blur_no_scale" )
		return CV_BLUR_NO_SCALE;
	assert( "unsupported filter type for Smooth module!!" && 0 );
}

void otSmoothModule::applyFilter(){
	cvSmooth(
			 (IplImage*)this->input->getData(),
			 this->output_buffer,
			 cv_smooth_type(this->property("filter").asString()),
			 this->property("size").asInteger()*2+1 //make sure its odd
			);
}


