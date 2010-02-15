#include <assert.h>

#include "highgui.h"

#include "otImageDisplayModule.h"
#include "otDataStream.h"

otImageDisplayModule::otImageDisplayModule(const char* name) {
	this->window_name = std::string(name);
}

otImageDisplayModule::~otImageDisplayModule(){
	cvDestroyWindow((char *)this->window_name.c_str());
}

void otImageDisplayModule::update(otDataStream *input) {
	// ensure that input data is IfiImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IfiImage" );

	// out input have been updated !
	this->input->lock();
	cvShowImage(this->window_name.c_str(), this->input->getData());
	this->input->unlock();
}

