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
#include "moEqualizeHistModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(EqualizeHist, "native", "EqualizeHist an image with one of several filters");

moEqualizeHistModule::moEqualizeHistModule() : moImageFilterModule8() {

	MODULE_INIT();

	// declare properties
}

moEqualizeHistModule::~moEqualizeHistModule() {
}

void moEqualizeHistModule::applyFilter(IplImage *src) {
	cvEqualizeHist(src, this->output_buffer);
}

