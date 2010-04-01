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


#include "moInvertModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Invert, "native", "Calculate the invert of an image");

moInvertModule::moInvertModule() {
	MODULE_INIT();
}

moInvertModule::~moInvertModule() {
}

void moInvertModule::applyFilter() {
	cvNot(static_cast<IplImage*>(this->input->getData()), this->output_buffer);
}


