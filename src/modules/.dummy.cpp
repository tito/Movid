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
#include "moDUMMYModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(DUMMY, "native", "DUMMY Description");

moDUMMYModule::moDUMMYModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties her, e.g:
	// this->properties["size"] = new moProperty(1.);
}

moDUMMYModule::~moDUMMYModule() {
}

void moDUMMYModule::applyFilter() {
	// Add your filter code here
}


