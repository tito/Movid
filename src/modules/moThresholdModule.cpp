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
	this->properties["threshold"]->setMin(0);
	this->properties["threshold"]->setMax(255);
	this->properties["adaptive"] = new moProperty(false);
	this->properties["block_size"] = new moProperty(21); // size fo neighbor hood to compare to for adaptive threshold

	this->properties["mode"] = new moProperty("mean");
	this->properties["mode"]->setChoices("mean;gaussian");

	this->properties["type"] = new moProperty("binary");
	this->properties["type"]->setChoices("binary;binary_inv;trunc;tozero;tozero_inv");

	// change the input/output type
	this->setInputType(0, "IplImage8");
	this->setOutputType(0, "IplImage8");
}

moThresholdModule::~moThresholdModule() 
{
}

void moThresholdModule::stop() {
	moImageFilterModule::stop();

	if ( this->output_buffer != NULL ) {
		cvReleaseImage(&this->output_buffer);
		this->output_buffer = NULL;
	}
}



int moThresholdModule::getCvType(const std::string &filter) 
{
	if ( filter == "binary" )
		return CV_THRESH_BINARY;
	if ( filter == "binary_inv" )
		return CV_THRESH_BINARY_INV;
	if ( filter == "trunc" )
		return CV_THRESH_TRUNC;
	if ( filter == "tozero" )
		return CV_THRESH_TOZERO;
	if ( filter == "tozero_inv" )
		return CV_THRESH_TOZERO_INV;

	LOGM(MO_ERROR,"Unsupported filter type: ");
	this->setError("Unsupported filter type");
	return 0;
}

int moThresholdModule::getCvAdaptativeType(const std::string &filter) 
{
	if ( filter == "binary" )
		return CV_THRESH_BINARY;
	if ( filter == "binary_inv" )
		return CV_THRESH_BINARY_INV;

	LOGM(MO_ERROR,"Unsupported filter type: ");
	this->setError("Unsupported filter type");
	return 0;
}

int moThresholdModule::getCvMode(const std::string &filter) 
{
	if ( filter == "mean" )
		return CV_ADAPTIVE_THRESH_MEAN_C;
	if ( filter == "gaussian" )
		return CV_ADAPTIVE_THRESH_GAUSSIAN_C;

	LOGM(MO_ERROR,"Unsupported filter type: ");
	this->setError("Unsupported filter type");
	return 0;
}

void moThresholdModule::applyFilter(IplImage *)
{
	IplImage* src = static_cast<IplImage*>(this->input->getData());

	if ( src->nChannels != 1 ) {
		this->setError("Threshold input image must be a single channel binary image.");
		this->stop();
		return;
	}

	if (this->property("adaptive").asBool()) 
	{
		int block_size = this->property("block_size").asInteger();
		
		//block size needs to be even (safeguard here)
		if (block_size % 2 == 0)
		{
			block_size++;
		}

		cvAdaptiveThreshold(
			src,
			this->output_buffer,
			255.0, //max value is output of where threshold was passed
			this->getCvMode(this->property("mode").asString()),
			this->getCvAdaptativeType(this->property("type").asString()),
			block_size,
			this->property("threshold").asDouble()*-1 //other way around on adpative, pass if src > (AVRG(block) - this arg)...so pixel pass if brighter than average neighboorhood + thresh (-1* -thresh)
		);
	} 
	else 
	{
		cvThreshold(
			src,
			this->output_buffer,
			this->property("threshold").asDouble(),
			255.0, //max value is output of where threshold was passed
			this->getCvType(this->property("type").asString())
		);
	}

}

