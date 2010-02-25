#include <assert.h>

#include "../otLog.h"
#include "../otModule.h"
#include "../otDataStream.h"
#include "otCombineModule.h"

MODULE_DECLARE(Combine, "native", "Take the maximum color from 2 image");

otCombineModule::otCombineModule() : otModule(OT_MODULE_INPUT|OT_MODULE_OUTPUT, 2, 1) {
	MODULE_INIT();

	this->input1 = NULL;
	this->input2 = NULL;
	this->output = new otDataStream("IplImage");
	this->output_buffer = NULL;
	this->need_update = false;

	// declare outputs
	this->input_infos[0] = new otDataStreamInfo(
			"image1", "IplImage", "Background image");
	this->input_infos[1] = new otDataStreamInfo(
			"image2", "IplImage", "Image to combine to background (black is transparent)");
	this->output_infos[1] = new otDataStreamInfo(
			"combine", "IplImage", "Result of the combine");
}

otCombineModule::~otCombineModule() {
}

void otCombineModule::stop() {
}

void otCombineModule::notifyData(otDataStream *input) {
	IplImage* src = (IplImage*)(input->getData());

	assert( input->getFormat() == "IplImage" );

	if ( src == NULL )
		return;

	if ( this->output_buffer == NULL ) {
		this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
		this->split = cvCreateImage(cvGetSize(src), src->depth, 1);
	} else {
		if ( this->output_buffer->width != src->width ||
			 this->output_buffer->height != src->height ) {
			LOG(CRITICAL) << "cannot combine image with different size";
		}
	}

	this->need_update = true;
}

void otCombineModule::update() {
	IplImage *d1 = NULL, *d2 = NULL;
	if ( this->need_update == false )
		return;
	if ( this->input1 == NULL || this->output_buffer == NULL )
		return;

	this->input1->lock();
	d1 = (IplImage *)this->input1->getData();
	if ( d1 == NULL ) {
		this->input1->unlock();
		return;
	}

	if ( this->input2 == NULL ) {
		cvCopy(d1, this->output_buffer);
	} else {
		this->input2->lock();
		d2 = (IplImage *)this->input2->getData();
		if ( d2 == NULL ) {
			this->input2->unlock();
			this->input1->unlock();
			return;
		}
		cvSplit(d2, this->split, NULL, NULL, NULL);
		cvCopy(d1, this->output_buffer);
		cvCopy(d2, this->output_buffer, this->split);
		this->input2->unlock();
	}

	this->output->push(this->output_buffer);
	this->input1->unlock();

	this->need_update = false;
}

void otCombineModule::setInput(otDataStream *stream, int n) {
	assert( n == 0 || n == 1 );
	if ( n == 0 ) {
		if ( this->input1 != NULL )
			this->input1->removeObserver(this);
		this->input1 = stream;
	} else {
		if ( this->input2 != NULL )
			this->input2->removeObserver(this);
		this->input2 = stream;
	}
	if ( stream != NULL )
		stream->addObserver(this);
}

otDataStream *otCombineModule::getInput(int n) {
	if ( n == 0 )
		return this->input1;
	if ( n == 1 )
		return this->input2;
	return NULL;
}

otDataStream *otCombineModule::getOutput(int n) {
	assert( n == 0 );
	return this->output;
}
