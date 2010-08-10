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
#include "moBackgroundSubtractModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(BackgroundSubtract, "native",
	"Subtracts the background from the current input image.\n" \
	"If 'toggle' is set to true, subtract each second frame.\n" \
	"Otherwise stores next frame as background once when 'recapture is set to true.'");

moBackgroundSubtractModule::moBackgroundSubtractModule() : moImageFilterModule() {

	MODULE_INIT();

	// declare properties
	this->properties["recapture"] = new moProperty(true);
	this->properties["toggle"] = new moProperty(false);
	this->properties["absolute"] = new moProperty(false);

	this->setInputType(0, "IplImage,IplImage8");
}

moBackgroundSubtractModule::~moBackgroundSubtractModule() {
}

void moBackgroundSubtractModule::stop() {
	moImageFilterModule::stop();

	if ( this->output_buffer != NULL ) {
		cvReleaseImage(&this->output_buffer);
		this->output_buffer = NULL;
	}
	if ( this->bg_buffer != NULL ) {
		cvReleaseImage(&this->bg_buffer);
		this->bg_buffer = NULL;
	}

	// reset state
	this->property("recapture").set(true);
}

void moBackgroundSubtractModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
	this->bg_buffer = cvCreateImage(cvGetSize(src),src->depth, src->nChannels);
	LOGM(MO_TRACE, "allocated output and background buffers");
}

void moBackgroundSubtractModule::applyFilter(IplImage *src) {
	assert( this->bg_buffer != NULL );
	assert( this->output_buffer != NULL );

	// check for recapture
	if (this->property("recapture").asBool()) {
		cvCopy(src, this->bg_buffer);
		this->property("recapture").set(false);
		LOGM(MO_TRACE, "recaptured background");
	} else {
		if (this->property("absolute").asBool())
		{
			// do absolute difference
			cvAbsDiff(src, this->bg_buffer, this->output_buffer);
		}
		else
		{
			// do subtraction
			cvSub(src, this->bg_buffer, this->output_buffer);
		}
		// check for next frame to recapture
		this->property("recapture").set(this->property("toggle").asBool());
	}
}

