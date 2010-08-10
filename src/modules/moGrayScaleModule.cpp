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
#include "moGrayScaleModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(GrayScale, "native", "Converts input image to a one bit channel image");

moGrayScaleModule::moGrayScaleModule() : moImageFilterModule(){
	MODULE_INIT();
	this->setOutputType(0, "IplImage8");
}

moGrayScaleModule::~moGrayScaleModule() {
}

void moGrayScaleModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, 1);	//only one channel
	LOG(MO_DEBUG, "allocated output buffer for GrayScale module.");
}

void moGrayScaleModule::applyFilter(IplImage *src) {
	cvCvtColor(src, this->output_buffer, CV_RGB2GRAY);
}

