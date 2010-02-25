#include <assert.h>
#include "moAmplifyModule.h"
#include "cv.h"

MODULE_DECLARE(Amplify, "native", "Amplifies input image (for evry pixel: p = p^amp, so lareg values get larger quicker");

moAmplifyModule::moAmplifyModule() : moImageFilterModule(){
	MODULE_INIT();
	this->properties["amplification"] = new moProperty(0.2);
}

moAmplifyModule::~moAmplifyModule() {
}

void moAmplifyModule::applyFilter() {
	IplImage* src = (IplImage*)(this->input->getData());
	cvMul( src,src, this->output_buffer, this->property("amplification").asDouble() );
}

