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
#include "moCannyModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Canny, "native", "Apply the Canny edge detector to an image");

moCannyModule::moCannyModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["lower_threshold"] = new moProperty(100);
	this->properties["lower_threshold"]->setMin(0);
	this->properties["lower_threshold"]->setMax(500);
	this->properties["upper_threshold"] = new moProperty(200);
	this->properties["upper_threshold"]->setMin(0);
	this->properties["upper_threshold"]->setMax(500);
}

moCannyModule::~moCannyModule() {
}

void moCannyModule::applyFilter() {
	cvCanny(
			static_cast<IplImage*>(this->input->getData()),
			this->output_buffer,
			this->property("lower_threshold").asInteger(),
			this->property("upper_threshold").asInteger(),
			3
			);
}


