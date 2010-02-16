#include <assert.h>
#include "otSmoothModule.h"
#include "cv.h"

MODULE_DECLARE(Smooth, "native", "Smooth an image with one of several filters");


otSmoothModule::otSmoothModule() : otImageFilterModule(){
	// declare properties
	this->properties["width"] = new otProperty(5.);
	this->properties["height"] = new otProperty(5.);
	this->properties["filter"] = new otProperty("gaussian");
}


void otSmoothModule::applyFilter(){
	std::string filter = this->property("filter").asString();
	int _filter = -1;

	if (filter == "median") { _filter = CV_MEDIAN; }
	if (filter == "gaussian") { _filter = CV_GAUSSIAN; }
	if (filter == "blur") { _filter = CV_BLUR; }
	if (filter == "blur_no_scale") { _filter = CV_BLUR_NO_SCALE; }
	if (filter == "bilateral") { /* Not yet supported */ }
	if (_filter == -1) { assert(0); }
		
	cvSmooth(
			 (IplImage*)this->input->getData(),
			 this->output_buffer, 
			 _filter,
			 this->property("width").asDouble(),
			 this->property("height").asDouble()
			);
}


