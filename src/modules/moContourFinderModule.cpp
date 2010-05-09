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
#include "moContourFinderModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(ContourFinder, "native", "ContourFinder finds contours in an image");

moContourFinderModule::moContourFinderModule() : moImageFilterModule(){

	MODULE_INIT();

	this->storage = cvCreateMemStorage(0);
}

moContourFinderModule::~moContourFinderModule() {
	cvReleaseMemStorage(&this->storage);
}

void moContourFinderModule::applyFilter(IplImage *src) {
	// Create a copy since cvFindContours will manipulate its input
	cvCopy(src, this->output_buffer);
	CvSeq* contours = 0;
	cvFindContours(this->output_buffer, this->storage, &contours);
	cvZero(this->output_buffer);
	if(contours)
		cvDrawContours(this->output_buffer, contours, cvScalarAll(255), cvScalarAll(255), 100);
}


