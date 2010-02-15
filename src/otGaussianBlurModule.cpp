
#include "otGaussianBlurModule.h"
#include "otDataStream.h"


otGaussianBlurModule::otGaussianBlurModule(): otModule(OT_MODULE_OUTPUT|OT_MODULE_INPUT, 1, 1){
	//FIXME make tehse properties (not ehave to be uneven number)
	this->width = 21;
	this->height = 21;
	this->output = NULL;
	this->smoothed = NULL;
}

otGaussianBlurModule::~otGaussianBlurModule(){
	//cvReleaseImage(&this->output);
}


void otGaussianBlurModule::setInput(otDataStream* in, int n){
	assert( input != NULL );
	assert( n == 0 );
	this->input = in;
	this->input->addObserver(this);
	this->output = new otDataStream("IplImage");

}

otDataStream* otGaussianBlurModule::getOutput(int n){
	return this->output;
}


void otGaussianBlurModule::notifyData(otDataStream *input) {
	// ensure that input data is IplImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" );
	
	this->input->lock();
	if (this->smoothed == NULL)
		this->smoothed = cvCreateImage(cvGetSize((IplImage*)(this->input->getData())), IPL_DEPTH_8U, 3);
	cvSmooth((IplImage*)this->input->getData(), this->smoothed, CV_GAUSSIAN, this->width, this->height);
	this->input->unlock();
	this->output->push(this->smoothed);
}

