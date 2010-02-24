#include <assert.h>
#include "otMirrorImageModule.h"
#include "cv.h"
#include <stdio.h>

MODULE_DECLARE(MirrorImage, "native", "Mirror an image. Flip it around x or y axis or both.");

otMirrorImageModule::otMirrorImageModule() : otImageFilterModule(){
	
	MODULE_INIT();

	this->properties["mirrorAxis"] = new otProperty("x");
}

otMirrorImageModule::~otMirrorImageModule() {
}

static int mirror_axis(std::string axis){
	if ( axis == "x" )
		return 0;
	if ( axis == "y" )
		return 1;
	if ( axis == "both" )
		return -1;
	std::cout << axis << std::endl;
	assert( "Unsupported mirror axis for MirrorImage module!!" && 0 );
	return 0;
}

void otMirrorImageModule::applyFilter(){
	cvFlip(
			 (IplImage*)this->input->getData(),
			 this->output_buffer,
			 mirror_axis(this->property("mirrorAxis").asString())
			 );
}


