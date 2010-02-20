#include "otDilateModule.h"
#include "cv.h"

MODULE_DECLARE(Dilate, "native", "Dilates the image (make bright regions bigger)");

otDilateModule::otDilateModule() {
	MODULE_INIT();
	this->properties["iterations"] = new otProperty(1);
}

otDilateModule::~otDilateModule() {
}

void otDilateModule::applyFilter(){
	IplImage* src = (IplImage*)this->input->getData();
	int iter = this->property("iterations").asInteger();
	cvErode(src, this->output_buffer, NULL, iter);
}


