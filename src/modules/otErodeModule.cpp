#include "otErodeModule.h"
#include "cv.h"

MODULE_DECLARE(Erode, "native", "Erodes the image (make bright regions smaller)");

otErodeModule::otErodeModule() {
	MODULE_INIT();
	this->properties["iterations"] = new otProperty(1);
}

otErodeModule::~otErodeModule() {
}

void otErodeModule::applyFilter(){
	IplImage* src = (IplImage*)this->input->getData();
	int iter = this->property("iterations").asInteger();
	cvErode(src, this->output_buffer, NULL, iter);
}


