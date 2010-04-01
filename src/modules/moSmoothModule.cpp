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
#include "moSmoothModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Smooth, "native", "Smooth an image with one of several filters");

moSmoothModule::moSmoothModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["size"] = new moProperty(1.);
	this->properties["size"]->setMin(0);
	this->properties["size"]->setMax(50);
	this->properties["filter"] = new moProperty("gaussian");
	this->properties["filter"]->setChoices("median;gaussian;blur;blur_no_scale");
}

moSmoothModule::~moSmoothModule() {
}

int moSmoothModule::toCvType(const std::string &filter) {
	if ( filter == "median" )
		return CV_MEDIAN;
	if ( filter == "gaussian" )
		return CV_GAUSSIAN;
	if ( filter == "blur" )
		return CV_BLUR;
	if ( filter == "blur_no_scale" )
		return CV_BLUR_NO_SCALE;

	LOGM(MO_ERROR) << "Unsupported filter type: " << filter;
	this->setError("Unsupported filter type");
	return 0;
}

void moSmoothModule::applyFilter() {
	cvSmooth(
		static_cast<IplImage*>(this->input->getData()),
		this->output_buffer,
		this->toCvType(this->property("filter").asString()),
		this->property("size").asInteger()*2+1 //make sure its odd
	);
}


