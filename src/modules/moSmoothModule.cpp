#include <assert.h>
#include "moSmoothModule.h"
#include "cv.h"

MODULE_DECLARE(Smooth, "native", "Smooth an image with one of several filters");

moSmoothModule::moSmoothModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["size"] = new moProperty(1.);
	this->properties["filter"] = new moProperty("gaussian");
}

moSmoothModule::~moSmoothModule() {
}

int moSmoothModule::toCvType(const std::string &filter) {
	if ( filter == "median" )
		return CV_MEDIAN;
	if ( filter == "gaussian" )
		return CV_GAUSSIAN;
	if ( filter == "blur" )
		return CV_BLUR;
	if ( filter == "blur_no_scale" )
		return CV_BLUR_NO_SCALE;

	LOGM(ERROR) << "Unsupported filter type: " << filter;
	this->setError("Unsupported filter type");
	return 0;
}

void moSmoothModule::applyFilter() {
	cvSmooth(
		static_cast<IplImage*>(this->input->getData()),
		this->output_buffer,
		this->toCvType(this->property("filter").asString()),
		this->property("size").asInteger()*2+1 //make sure its odd
	);
}


