#include <assert.h>

#include "otLog.h"
#include "otGaussianBlurModule.h"
#include "otDataStream.h"

LOG_DECLARE("GaussianBlur");

otGaussianBlurModule::otGaussianBlurModule() :
	otModule(OT_MODULE_OUTPUT|OT_MODULE_INPUT, 1, 1)
{
	this->output = new otDataStream("IplImage");
	this->smoothed = NULL;

	// declare input/output
	this->input_names[0] = std::string("image");
	this->input_types[0] = std::string("IplImage");
	this->output_names[0] = std::string("image");
	this->output_types[0] = std::string("IplImage");

	// declare properties
	this->properties["width"] = new otProperty(5.);
	this->properties["height"] = new otProperty(5.);
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
		LOG(DEBUG) << "First time, create the image for smooth";
		this->smoothed = cvCreateImage(
			cvGetSize((IplImage*)(this->input->getData())),
			IPL_DEPTH_8U, 3);

		LOG(INFO) << "Gaussian created with width=" <<
			this->property("width").asDouble() << ", height=" <<
			this->property("height").asDouble();
	}

	// FIXME prefer to do it in update, notifyData can be called inside a thread
	cvSmooth((IplImage*)this->input->getData(),
			this->smoothed, CV_GAUSSIAN,
			this->property("width").asDouble(),
			this->property("height").asDouble());

	this->input->unlock();

	this->output->push(this->smoothed);
}

void otGaussianBlurModule::update() {
}

