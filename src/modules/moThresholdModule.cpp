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
#include "moThresholdModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Threshold, "native", "Thresholding to throw away all values below or above certain threshold");

moThresholdModule::moThresholdModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["threshold"] = new moProperty(50);
	this->properties["adaptive"] = new moProperty(false);
	this->properties["block_size"] = new moProperty(21); // size fo neighbor hood to compare to for adaptive threshold
}

moThresholdModule::~moThresholdModule() {
}

void moThresholdModule::applyFilter(){
	IplImage* src = static_cast<IplImage*>(this->input->getData());

	if ( src->nChannels != 1 ) {
		this->setError("Threshold input image must be a single channel binary image.");
		this->stop();
		return;
	}

	if (this->property("adaptive").asBool()) {
		cvAdaptiveThreshold(
			src,
			this->output_buffer,
			255.0, //max value is output of where threshold was passed
			CV_ADAPTIVE_THRESH_MEAN_C,
			CV_THRESH_BINARY,
			this->property("block_size").asInteger(),
			this->property("threshold").asDouble()*-1 //other way around on adpative, pass if src > (AVRG(block) - this arg)...so pixel pass if brighter than average neighboorhood + thresh (-1* -thresh)
		);
	} else {
		cvThreshold(
			src,
			this->output_buffer,
			this->property("threshold").asDouble(),
			255.0, //max value is output of where threshold was passed
			CV_THRESH_BINARY
		);
	}

}


