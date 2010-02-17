#include <assert.h>
#include "otHighpassModule.h"
#include "cv.h"

MODULE_DECLARE(Highpass, "native", "Highpass filter");

otHighpassModule::otHighpassModule() : otImageFilterModule(){
	MODULE_INIT();	
	// declare properties
	this->properties["size"] = new otProperty(2);
	this->properties["blur"] = new otProperty(2);

}

otHighpassModule::~otHighpassModule() {
}

void otHighpassModule::applyFilter(){
	int b1 = this->property("size").asInteger()*2+1; //make sure its odd
	int b2 = this->property("blur").asInteger()*2+1; //make sure its odd
	IplImage* src = (IplImage*)this->input->getData();
	cvSmooth(src, this->output_buffer, CV_GAUSSIAN, b1);
	cvSub(src, this->output_buffer, this->output_buffer);
	cvSmooth(this->output_buffer, this->output_buffer, CV_GAUSSIAN, b2);
}


