#include <sstream>
#include <assert.h>

#include "highgui.h"

#include "otImageDisplayModule.h"
#include "../otDataStream.h"

MODULE_DECLARE(ImageDisplay, "native", "Display image on a window");

static unsigned int count = 0;

otImageDisplayModule::otImageDisplayModule() : otModule(OT_MODULE_INPUT, 1, 0) {

	MODULE_INIT();

	this->input = NULL;

	// declare inputs
	this->input_names[0] = std::string("image");
	this->input_types[0] = std::string("IplImage");

	// declare properties
	std::ostringstream oss;
	oss << "OpenTracker" << (count++);
	this->properties["name"] = new otProperty(oss.str());
}

otImageDisplayModule::~otImageDisplayModule(){
}

void otImageDisplayModule::stop() {
	cvDestroyWindow(this->property("name").asString().c_str());
	otModule::stop();
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
	assert( n == 0 );
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = input;
	if ( this->input != NULL )
		this->input->addObserver(this);
}

otDataStream* otImageDisplayModule::getInput(int n) {
	assert( n == 0);
	return this->input;
}

otDataStream* otImageDisplayModule::getOutput(int n) {
	assert( "otImageDisplayModule don't accept output" && 0 );
	return NULL;
}

void otImageDisplayModule::update() {
}

