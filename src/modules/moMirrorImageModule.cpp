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


#include "moMirrorImageModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(MirrorImage, "native", "Mirror an image. Flip it around x or y axis or both.");

moMirrorImageModule::moMirrorImageModule() : moImageFilterModule(){

	MODULE_INIT();

	this->properties["mirrorAxis"] = new moProperty("x");
}

moMirrorImageModule::~moMirrorImageModule() {
}

int moMirrorImageModule::toCvType(const std::string &axis) {
	if ( axis == "x" )
		return 0;
	if ( axis == "y" )
		return 1;
	if ( axis == "both" )
		return -1;

	LOGM(MO_ERROR, "unsupported axis: " << axis);
	this->setError("Unsuported mirror axis");
	return 0;
}

void moMirrorImageModule::applyFilter(IplImage *src) {
	cvFlip(
		src,
		this->output_buffer,
		this->toCvType(this->property("mirrorAxis").asString())
	);
}


