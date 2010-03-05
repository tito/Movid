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

void moImageDisplayModule::setInput(moDataStream *stream, int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return;
	}

	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( stream != NULL ) {
		if ( stream->getFormat() != "IplImage" ) {
			this->setError("Input 0 accept only IplImage");
			this->input = NULL;
			return;
		}
	}
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moImageDisplayModule::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moImageDisplayModule::getOutput(int n) {
	this->setError("no output supported");
	return NULL;
}

void moImageDisplayModule::update() {
}

