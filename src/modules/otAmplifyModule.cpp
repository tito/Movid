#include <assert.h>
#include "otAmplifyModule.h"
#include "cv.h"

MODULE_DECLARE(Amplify, "native", "Amplifies input image (for evry pixel: p = p^amp, so lareg values get larger quicker");

otAmplifyModule::otAmplifyModule() : otImageFilterModule(){
	MODULE_INIT();
	this->properties["amplification"] = new otProperty(0.2);
}

otAmplifyModule::~otAmplifyModule() {
}

void otAmplifyModule::applyFilter() {
	IplImage* src = (IplImage*)(this->input->getData());
	cvMul( src,src, this->output_buffer, this->property("amplification").asDouble() );
}

