#include <sstream>
#include <assert.h>

#include "highgui.h"

#include "moImageDisplayModule.h"
#include "../moDataStream.h"

MODULE_DECLARE(ImageDisplay, "native", "Display image on a window");

static unsigned int count = 0;

moImageDisplayModule::moImageDisplayModule() : moModule(MO_MODULE_INPUT, 1, 0) {

	MODULE_INIT();

	this->input = NULL;

	// declare inputs
	this->input_infos[0] = new moDataStreamInfo(
			"image", "IplImage", "Show image stream in a window");

	// declare properties
	std::ostringstream oss;
	oss << "Movid" << (count++);
	this->properties["name"] = new moProperty(oss.str());
}

moImageDisplayModule::~moImageDisplayModule(){
}

void moImageDisplayModule::stop() {
	cvDestroyWindow(this->property("name").asString().c_str());
	moModule::stop();
}

void moImageDisplayModule::notifyData(moDataStream *input) {
	// ensure that input data is IfiImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" );

	// out input have been updated !
	this->input->lock();
	cvShowImage(this->property("name").asString().c_str(), this->input->getData());
	this->input->unlock();
}

void moImageDisplayModule::setInput(moDataStream *input, int n) {
	assert( n == 0 );
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = input;
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moImageDisplayModule::getInput(int n) {
	assert( n == 0);
	return this->input;
}

moDataStream* moImageDisplayModule::getOutput(int n) {
	assert( "moImageDisplayModule don't accept output" && 0 );
	return NULL;
}

void moImageDisplayModule::update() {
}

