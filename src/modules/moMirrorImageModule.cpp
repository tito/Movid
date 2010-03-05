#include <assert.h>
#include "moMirrorImageModule.h"
#include "cv.h"
#include <stdio.h>

MODULE_DECLARE(MirrorImage, "native", "Mirror an image. Flip it around x or y axis or both.");

moMirrorImageModule::moMirrorImageModule() : moImageFilterModule(){

	MODULE_INIT();

	this->properties["mirrorAxis"] = new moProperty("x");
}

moMirrorImageModule::~moMirrorImageModule() {
}

int moMirrorImageModule::toCvType(const std::string &axis) {
	if ( axis == "x" )
		return 0;
	if ( axis == "y" )
		return 1;
	if ( axis == "both" )
		return -1;

	LOGM(ERROR) << "unsupported axis: " << axis;
	this->setError("Unsuported mirror axis");
	return 0;
}

void moMirrorImageModule::applyFilter(){
	cvFlip(
		static_cast<IplImage*>(this->input->getData()),
		this->output_buffer,
		this->toCvType(this->property("mirrorAxis").asString())
	);
}


