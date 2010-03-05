#include <assert.h>
#include "moImageFilterModule.h"
#include "../moLog.h"
#include "../moDataStream.h"

LOG_DECLARE("ImageFilter");

moImageFilterModule::moImageFilterModule() :
	moModule(MO_MODULE_OUTPUT|MO_MODULE_INPUT, 1, 1)
{
	this->input = NULL;
	this->output = new moDataStream("IplImage");
	this->output_buffer = NULL;
	this->need_update = false; //dont update until we get data

	// declare input/output
	this->input_infos[0] = new moDataStreamInfo("image", "IplImage", "Input image stream");
	this->output_infos[0] = new moDataStreamInfo("image", "IplImage", "Output image stream");
}

moImageFilterModule::~moImageFilterModule() {
	delete this->output;
	if ( this->output_buffer != NULL )
		cvReleaseImage(&this->output_buffer);
}

void moImageFilterModule::setInput(moDataStream* stream, int n) {
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

moDataStream* moImageFilterModule::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moImageFilterModule::getOutput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid output index");
		return NULL;
	}
	return this->output;
}

void moImageFilterModule::stop() {
	if ( this->output_buffer != NULL ) {
		cvReleaseImage(&this->output_buffer);
		this->output_buffer = NULL;
	}
	this->need_update = false;
	moModule::stop();
}

void moImageFilterModule::notifyData(moDataStream *input) {
	// ensure that input data is IplImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" );

	// FIXME  also do if size, nChannles or depth has changed
	if ( this->output_buffer == NULL ) {
		input->lock();
		this->allocateBuffers();
		input->unlock();
	}

	this->need_update = true;
}


void moImageFilterModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	LOGM(DEBUG) << "First time, allocating output buffer for image filter";
	this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
}

void moImageFilterModule::update() {
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

