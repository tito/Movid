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


#include "moAmplifyModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Amplify, "native", "Amplifies input image (for every pixel: p = p^amp, so larger values get larger quicker");

moAmplifyModule::moAmplifyModule() : moImageFilterModule(){
	MODULE_INIT();
	this->properties["amplification"] = new moProperty(0.2);
}

moAmplifyModule::~moAmplifyModule() {
}

void moAmplifyModule::applyFilter() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	cvMul(src, src, this->output_buffer, this->property("amplification").asDouble());
}

