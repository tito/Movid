/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#include <assert.h>
#include "moImageFilterModule.h"
#include "../moLog.h"
#include "../moDataStream.h"

LOG_DECLARE("ImageFilter");

moImageFilterModule::moImageFilterModule() :
	moModule(MO_MODULE_OUTPUT|MO_MODULE_INPUT)
{
	this->input = NULL;
	this->output = new moDataStream("IplImage");
	this->output_dispatcher = this->output;
	this->output_buffer = NULL;

	// declare input/output
	this->declareInput(0, &this->input, new moDataStreamInfo(
			"image", "IplImage", "Input image stream"));
	this->declareOutput(0, &this->output_dispatcher,  new moDataStreamInfo(
			"image", "IplImage", "Output image stream"));
}

moImageFilterModule::~moImageFilterModule() {
	delete this->output;
	if ( this->output_buffer != NULL )
		cvReleaseImage(&this->output_buffer);
}

void moImageFilterModule::stop() {
	moModule::stop();
	if ( this->output_buffer != NULL ) {
		cvReleaseImage(&this->output_buffer);
		this->output_buffer = NULL;
	}
}

void moImageFilterModule::notifyData(moDataStream *input) {
	// ensure that input data is IplImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" || input->getFormat() == "IplImage8" );

	IplImage* src = static_cast<IplImage*>(this->input->getData());

	// FIXME  also do if size, nChannels or depth has changed
	if ( this->needBufferAllocation() ) {
		input->lock();
		this->allocateBuffers();
		input->unlock();
	}

	this->notifyUpdate();
}

bool moImageFilterModule::needBufferAllocation() {
	return (this->output_buffer == NULL) ? true : false;
}

void moImageFilterModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	LOGM(MO_DEBUG, "Allocating output buffer for image filter");
	if ( this->output_buffer != NULL )
		cvReleaseImage(&this->output_buffer);
	this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
}

void moImageFilterModule::update() {
	IplImage *dup = NULL;

	if ( this->input == NULL )
		return;

	this->input->lock();

	// don't pass data to filter if source is NULL
	if ( this->input->getData() != NULL ) {
		// duplicate the image, and release as fast as we can the input lock.
		dup = cvCloneImage(static_cast<IplImage *>(this->input->getData()));
		this->input->unlock();

		// apply the filter
		this->applyFilter(dup);

		// release the duplicated image
		cvReleaseImage(&dup);

		// push the new data
		this->output_dispatcher->push(this->output_buffer);
	} else {
		this->input->unlock();
	}

}

//
// ImageFilter that handle IplImage and IplImage8
//
moImageFilterModule8::moImageFilterModule8() : moImageFilterModule() {
	this->output8 = new moDataStream("IplImage8");
	this->setInputType(0, "IplImage,IplImage8");
	this->declareOutput(1, &this->output8,  new moDataStreamInfo(
			"image", "IplImage8", "Output image stream (1 channel)"));
}

moImageFilterModule8::~moImageFilterModule8() {
	delete this->output8;
}

void moImageFilterModule8::allocateBuffers() {
	moImageFilterModule::allocateBuffers();
	if ( this->output_buffer == NULL )
		return;

	// if we are using a 8 bits image (1 channel)
	// move the data to another output port
	if ( this->output_buffer->nChannels == 1 )
		this->output_dispatcher = this->output8;
	else
		this->output_dispatcher = this->output;
}

bool moImageFilterModule8::needBufferAllocation() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( moImageFilterModule::needBufferAllocation() )
		return true;
	if ( this->output_buffer->nChannels != src->nChannels )
		return true;
	return false;
}

