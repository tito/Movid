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


#include <map>
#include <utility>
#include <assert.h>
#include "moDistanceTransformModule.h"
#include "../moLog.h"
#include "cv.h"
#include <highgui.h>

MODULE_DECLARE(DistanceTransform, "native", "Assign each pixel a color representing pixel's distance to the closest contour.");

moDistanceTransformModule::moDistanceTransformModule() : moImageFilterModule(){

	MODULE_INIT();

	// The factor by which the resulting image is scaled (for visibility)
	this->properties["scale"] = new moProperty(5);
	this->properties["scale"]->setMin(1);
	this->properties["scale"]->setMax(255);
	this->properties["mask_size"] = new moProperty("5");
	this->properties["mask_size"]->setChoices("3;5;Precise");
	this->properties["metric"] = new moProperty("L2");
	this->properties["metric"]->setChoices("L2;L1;L12;Fair;Welsch");
}

moDistanceTransformModule::~moDistanceTransformModule() {
}

void moDistanceTransformModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	// Formats required by cvDistTransform:
	// Converted version of the input img
	this->converted = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	// The img that will contain the actual distances
	this->dist = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, 1);
	this->output_buffer = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	LOG(MO_DEBUG) << "allocated output buffer for DistanceTransform module.";
}

int moDistanceTransformModule::toCvType(const std::string &metric) {
	if ( metric == "L2" )
		return CV_DIST_L2;
	if ( metric == "L1" )
		return CV_DIST_L1;
	if ( metric == "L12" )
		return CV_DIST_L12;
	if ( metric == "Fair" )
		return CV_DIST_FAIR;
	if ( metric == "Welsch" )
		return CV_DIST_WELSCH;

	LOGM(MO_ERROR) << "Unsupported distance metric: " << metric;
	this->setError("Unsupported distance metric");
	return 0;
}

int moDistanceTransformModule::toCvMaskSize(const std::string &mask_size) {
	if ( mask_size == "3" )
		return 3;
	if ( mask_size == "5" )
		return 5;
	if ( mask_size == "Precise" )
		return CV_DIST_MASK_PRECISE;

	LOGM(MO_ERROR) << "Unsupported mask size for distance transform: " << mask_size;
	this->setError("Unsupported mask size for distance transform");
	return 0;
}

void moDistanceTransformModule::applyFilter() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	cvConvertImage(src, this->converted);

	cvDistTransform(
			 this->converted,
			 this->dist,
			 this->toCvType(this->property("metric").asString()),
			 this->toCvMaskSize(this->property("mask_size").asString())
			 );
	// In order to actually see something in the output, we have to scale the
	// result for visibility.
	cvConvertScale(this->dist, this->output_buffer,
				   this->property("scale").asInteger(), 0);
}
