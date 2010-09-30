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


#include "moAdvancedBGSubModule.h"



MODULE_DECLARE(AdvancedBGSub, "native", "AdvancedBGSub an image with one of several filters");

moAdvancedBGSubModule::moAdvancedBGSubModule() : moImageFilterModule8() {

	MODULE_INIT();

	// declare properties
	this->initialized = false;
}

moAdvancedBGSubModule::~moAdvancedBGSubModule() {
}

void moAdvancedBGSubModule::applyFilter(IplImage *src) {
	if (!this->initialized) {
		this->params.SetFrameSize(src->width, src->height);
		this->params.LowThreshold() = 15*15;
		this->params.HighThreshold() = 2* this->params.LowThreshold();	// Note: high threshold is used by post-processing 
		this->params.HistorySize() = 100;
		this->params.EmbeddedDim() = 20;

		this->bgs.Initalize(this->params);
	}
}

