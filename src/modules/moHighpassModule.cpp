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


#include "moHighpassModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Highpass, "native", "Highpass filter");

moHighpassModule::moHighpassModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["size"] = new moProperty(2);
	this->properties["blur"] = new moProperty(2);

}

moHighpassModule::~moHighpassModule() {
}

void moHighpassModule::applyFilter(){
	int b1 = this->property("size").asInteger()*2+1; //make sure its odd
	int b2 = this->property("blur").asInteger()*2+1; //make sure its odd
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	cvSmooth(src, this->output_buffer, CV_GAUSSIAN, b1);
	cvSub(src, this->output_buffer, this->output_buffer);
	cvSmooth(this->output_buffer, this->output_buffer, CV_GAUSSIAN, b2);
}


