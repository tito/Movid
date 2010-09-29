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

#include "../moLog.h"
#include "../moModule.h"
#include "../moDataStream.h"
#include "moBinaryMaskModule.h"

MODULE_DECLARE(BinaryMask, "native", "Everything that's black in the second image is black in the output image. Rest unchanged.");

moBinaryMaskModule::moBinaryMaskModule() : moModule(MO_MODULE_INPUT|MO_MODULE_OUTPUT) {
	MODULE_INIT();

	this->input = NULL;
	this->mask = NULL;
	this->output = new moDataStream("IplImage8");
	this->output_buffer = NULL;

	// declare inputs/output
	this->declareInput(0, &this->input, new moDataStreamInfo(
			"image", "IplImage8", "Original image"));
	this->declareInput(1, &this->mask, new moDataStreamInfo(
			"mask", "IplImage8", "Binary mask image."));
	this->declareOutput(0, &this->output, new moDataStreamInfo(
			"masked", "IplImage8", "Result of the masking."));
}

moBinaryMaskModule::~moBinaryMaskModule() {
}

void moBinaryMaskModule::notifyData(moDataStream *input) {
	IplImage* src = static_cast<IplImage*>(input->getData());
	assert( input->getFormat() == "IplImage8" );
	if ( src == NULL )
		return;

	if ( this->output_buffer == NULL ) {
		this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
		this->split = cvCreateImage(cvGetSize(src), src->depth, 1);
	} else {
		if ( this->output_buffer->width != src->width ||
			 this->output_buffer->height != src->height ) {
			LOG(MO_CRITICAL, "cannot mask images with different sizes");
		}
	}

	this->notifyUpdate();
}

void moBinaryMaskModule::update() {
	IplImage *d1 = NULL, *d2 = NULL;
	if ( this->input == NULL || this->output_buffer == NULL )
		return;

	this->input->lock();
	d1 = (IplImage *)this->input->getData();
	if ( d1 == NULL ) {
		this->input->unlock();
		return;
	}
	// XXX clone still necessary? see also below
	d1 = cvCloneImage(d1);
	this->input->unlock();

	if ( this->mask == NULL )
		return;
	else {
		this->mask->lock();
		d2 = (IplImage *)this->mask->getData();
		if ( d2 == NULL ) {
			this->mask->unlock();
			cvReleaseImage(&d1);
			return;
		}
		d2 = cvCloneImage(d2);
		this->mask->unlock();

	cvAnd(d1, d2, this->output_buffer);
	}

	this->output->push(this->output_buffer);

	cvReleaseImage(&d1);
	cvReleaseImage(&d2);
}
