#include <assert.h>

#include "highgui.h"

#include "otImageDisplayModule.h"
#include "../otDataStream.h"

MODULE_DECLARE(ImageDisplay, "native", "Display image on a window");

otImageDisplayModule::otImageDisplayModule() : otModule(OT_MODULE_INPUT, 1, 0) {

	// declare inputs
	this->input_names[0] = std::string("image");
	this->input_types[0] = std::string("IplImage");

	// declare properties
	this->properties["name"] = new otProperty("OpenTracker");
}

otImageDisplayModule::~otImageDisplayModule(){
	cvDestroyWindow(this->property("name").asString().c_str());
}

void otImageDisplayModule::notifyData(otDataStream *input) {
	// ensure that input data is IfiImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" );

	// out input have been updated !
	this->input->lock();
	cvShowImage(this->property("name").asString().c_str(), this->input->getData());
	this->input->unlock();
}

void otImageDisplayModule::setInput(otDataStream *input, int n) {
	assert( input != NULL );
	assert( n == 0 );
	this->input = input;
	this->input->addObserver(this);
}

otDataStream* otImageDisplayModule::getOutput(int n) {
	assert( "otImageDisplayModule don't accept output" && 0 );
	return NULL;
}

void otImageDisplayModule::update() {
}

