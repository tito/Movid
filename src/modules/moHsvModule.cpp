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
#include "moHsvModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Hsv, "native", "Hsv filter (can be used for color tracking)");

moHsvModule::moHsvModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties her, e.g:
	this->properties["hmin"] = new moProperty(0);
	this->properties["hmin"]->setMin(0);
	this->properties["hmin"]->setMax(255);
	this->properties["hmax"] = new moProperty(20);
	this->properties["hmax"]->setMin(0);
	this->properties["hmax"]->setMax(255);
	this->properties["smin"] = new moProperty(0);
	this->properties["smin"]->setMin(0);
	this->properties["smin"]->setMax(255);
	this->properties["smax"] = new moProperty(20);
	this->properties["smax"]->setMin(0);
	this->properties["smax"]->setMax(255);
	this->properties["vmin"] = new moProperty(0);
	this->properties["vmin"]->setMin(0);
	this->properties["vmin"]->setMax(255);
	this->properties["vmax"] = new moProperty(255);
	this->properties["vmax"]->setMin(0);
	this->properties["vmax"]->setMax(255);
}

moHsvModule::~moHsvModule() {
}

void moHsvModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, 1);	//only one channel
	LOG(MO_DEBUG, "allocated output buffer for Hsv module.");
}

void moHsvModule::applyFilter(IplImage *image) {
	IplImage *mask = this->output_buffer;
	IplImage* hsv = cvCloneImage(image);
	int hmin = this->property("hmin").asInteger(),
		hmax = this->property("hmax").asInteger(),
		smin = this->property("smin").asInteger(),
		smax = this->property("smax").asInteger(),
		vmin = this->property("vmin").asInteger(),
		vmax = this->property("vmax").asInteger();

	cvCvtColor(image, hsv, CV_BGR2HSV);
	cvInRangeS(hsv, cvScalar(hmin, smin, vmin), cvScalar(hmax, smax, vmax), mask);
	cvReleaseImage(&hsv);

}


