#include "moInvertModule.h"
#include "cv.h"

MODULE_DECLARE(Invert, "native", "Calculate the invert of an image");

moInvertModule::moInvertModule() {
	MODULE_INIT();
}

moInvertModule::~moInvertModule() {
}

void moInvertModule::applyFilter(){
	cvNot((IplImage*)this->input->getData(), this->output_buffer);
}


