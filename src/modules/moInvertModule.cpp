#include "moInvertModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Invert, "native", "Calculate the invert of an image");

moInvertModule::moInvertModule() {
	MODULE_INIT();
}

moInvertModule::~moInvertModule() {
}

void moInvertModule::applyFilter() {
	cvNot(static_cast<IplImage*>(this->input->getData()), this->output_buffer);
}


