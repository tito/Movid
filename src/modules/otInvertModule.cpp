#include "otInvertModule.h"
#include "cv.h"

MODULE_DECLARE(Invert, "native", "Calculate the invert of an image");

void otInvertModule::applyFilter(){
	cvNot((IplImage*)this->input->getData(), this->output_buffer);
}


