#include "moDilateModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Dilate, "native", "Dilates the image (make bright regions bigger)");

moDilateModule::moDilateModule() {
	MODULE_INIT();
	this->properties["iterations"] = new moProperty(1);
}

moDilateModule::~moDilateModule() {
}

void moDilateModule::applyFilter(){
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	int iter = this->property("iterations").asInteger();
	cvErode(src, this->output_buffer, NULL, iter);
}


