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
#include "moYCrCbThresholdModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(YCrCbThreshold, "native", "YCrCbThreshold filter (can be used for color tracking)");

moYCrCbThresholdModule::moYCrCbThresholdModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties her, e.g:
	this->properties["Y_min"] = new moProperty(0);
	this->properties["Y_min"]->setMin(0);
	this->properties["Y_min"]->setMax(255);
	this->properties["Y_max"] = new moProperty(255);
	this->properties["Y_max"]->setMin(0);
	this->properties["Y_max"]->setMax(255);
	this->properties["Cr_min"] = new moProperty(145);
	this->properties["Cr_min"]->setMin(0);
	this->properties["Cr_min"]->setMax(255);
	this->properties["Cr_max"] = new moProperty(175);
	this->properties["Cr_max"]->setMin(0);
	this->properties["Cr_max"]->setMax(255);
	this->properties["Cb_min"] = new moProperty(100);
	this->properties["Cb_min"]->setMin(0);
	this->properties["Cb_min"]->setMax(255);
	this->properties["Cb_max"] = new moProperty(130);
	this->properties["Cb_max"]->setMin(0);
	this->properties["Cb_max"]->setMax(255);

	this->setOutputType(0, "IplImage8");
}

moYCrCbThresholdModule::~moYCrCbThresholdModule() {
}

void moYCrCbThresholdModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, 1);	//only one channel
}

void moYCrCbThresholdModule::applyFilter(IplImage *image) {
	IplImage* ycrcb = cvCloneImage(image);
	int ymin = this->property("Y_min").asInteger(),
		ymax = this->property("Y_max").asInteger(),
		crmin = this->property("Cr_min").asInteger(),
		crmax = this->property("Cr_max").asInteger(),
		cbmin = this->property("Cb_min").asInteger(),
		cbmax = this->property("Cb_max").asInteger();

	cvCvtColor(image, ycrcb, CV_BGR2YCrCb);
	cvInRangeS(ycrcb, cvScalar(ymin, crmin, cbmin),
			   cvScalar(ymax, crmax, cbmax), this->output_buffer);
	cvReleaseImage(&ycrcb);
}

