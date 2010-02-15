#include <assert.h>

#include "otGaussianBlurModule.h"
#include "otDataStream.h"


otGaussianBlurModule::otGaussianBlurModule() :
	otModule(OT_MODULE_OUTPUT|OT_MODULE_INPUT, 1, 1)
{
	//FIXME make tehse properties (not ehave to be uneven number)
	this->width = 21;
	this->height = 21;
	this->output = new otDataStream("IplImage");
	this->smoothed = NULL;

	this->input_names[0] = std::string("image");
	this->input_types[0] = std::string("IplImage");
	this->output_names[0] = std::string("image");
	this->output_types[0] = std::string("IplImage");
}

otGaussianBlurModule::~otGaussianBlurModule(){
}

void otGaussianBlurModule::setInput(otDataStream* stream, int n) {
	assert( stream != NULL );
	assert( n == 0 );

	this->input = stream;
	this->input->addObserver(this);
}

otDataStream* otGaussianBlurModule::getOutput(int n){
	assert( n == 0);
	return this->output;
}

void otGaussianBlurModule::notifyData(otDataStream *input) {
	// ensure that input data is IplImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" );

	this->input->lock();

	if (this->smoothed == NULL) {
		this->smoothed = cvCreateImage(
			cvGetSize((IplImage*)(this->input->getData())),
			IPL_DEPTH_8U, 3);
	}

	// FIXME prefer to do it in update, notifyData can be called inside a thread
	cvSmooth((IplImage*)this->input->getData(), this->smoothed, CV_GAUSSIAN, this->width, this->height);

	this->input->unlock();

	this->output->push(this->smoothed);
}

