#include "otInvertModule.h"
#include "cv.h"

void otInvertModule::applyFilter(){
	cvNot((IplImage*)this->input->getData(), this->output_buffer);
}


