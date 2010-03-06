#include "moErodeModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Erode, "native", "Erodes the image (make bright regions smaller)");

moErodeModule::moErodeModule() {
	MODULE_INIT();
	this->properties["iterations"] = new moProperty(1);
}

moErodeModule::~moErodeModule() {
}

void moErodeModule::applyFilter(){
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	int iter = this->property("iterations").asInteger();
	cvErode(src, this->output_buffer, NULL, iter);
}


