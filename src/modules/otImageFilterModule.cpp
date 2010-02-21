#include <assert.h>
#include "otImageFilterModule.h"
#include "../otLog.h"
#include "../otDataStream.h"

LOG_DECLARE("ImageFilter");

otImageFilterModule::otImageFilterModule() :
	otModule(OT_MODULE_OUTPUT|OT_MODULE_INPUT, 1, 1)
{
	this->input = NULL;
	this->output = new otDataStream("IplImage");
	this->output_buffer = NULL;
	this->need_update = false; //dont update until we get data

	// declare input/output
	this->input_names[0] = std::string("image");
	this->input_types[0] = std::string("IplImage");
	this->output_names[0] = std::string("image");
	this->output_types[0] = std::string("IplImage");
}

otImageFilterModule::~otImageFilterModule() {
}

void otImageFilterModule::setInput(otDataStream* stream, int n) {
	assert( n == 0 );
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( this->input != NULL )
		this->input->addObserver(this);
}

otDataStream* otImageFilterModule::getInput(int n) {
	assert(n == 0);
	return this->input;
}

otDataStream* otImageFilterModule::getOutput(int n) {
	assert(n == 0);
	return this->output;
}

void otImageFilterModule::stop() {
	if ( this->output_buffer != NULL ) {
		cvReleaseImage(&this->output_buffer);
		this->output_buffer = NULL;
	}
	this->need_update = false;
	otModule::stop();
}

void otImageFilterModule::notifyData(otDataStream *input) {
	// ensure that input data is IplImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" );

	//FIXME  also do if size, nChannles or depth has changed
	if ( this->output_buffer == NULL )
		this->allocateBuffers();

	this->need_update = true;
}


void otImageFilterModule::allocateBuffers() {
	IplImage* src = (IplImage*)(this->input->getData());
	if ( src == NULL )
		return;
	LOG(DEBUG) << "First time, allocating output buffer for image filter";
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
}

void otImageFilterModule::update() {
	if ( this->input == NULL )
		return;
	if ( this->need_update ) {
		this->input->lock();

		// don't pass data to filter if source is NULL
		if ( this->input->getData() != NULL ) {
			this->applyFilter();
			this->output->push(this->output_buffer);
		}

		this->input->unlock();
	}
}

