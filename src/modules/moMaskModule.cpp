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
#include "moMaskModule.h"
#include "../moLog.h"
#include "cv.h"
#include "highgui.h"

MODULE_DECLARE(Mask, "native", "Mask Description");

void internal_saveas_cb(moProperty *prop, void *_inst) {
	moMaskModule *inst = static_cast<moMaskModule*> (_inst);
	inst->save_data = true;
}

void maskfileChangedCallback(moProperty *prop, void *_inst) {
	moMaskModule *inst = static_cast<moMaskModule*> (_inst);
	inst->reloadMask();
}


moMaskModule::moMaskModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["saveas"] = new moProperty("");
	this->properties["saveas"]->addCallback(internal_saveas_cb, this);
	this->properties["filename"] = new moProperty("");
	this->properties["filename"]->addCallback(maskfileChangedCallback, this);

	this->mask_buffer = NULL;
	this->save_data = false;
}

moMaskModule::~moMaskModule() {
}

void moMaskModule::applyFilter() {
	assert( this->output_buffer != NULL );

	IplImage* src = (IplImage*)(this->input->getData());
	if ( src == NULL )
		return;

	if (this->save_data) {
		LOGM(MO_INFO) << "saving current image as " << this->property("saveas").asString();
		cvSaveImage(this->property("saveas").asString().c_str(), src);
		this->save_data = false;
	}

	// do masking
	if (this->mask_buffer != NULL) {
		cvAnd(src, this->mask_buffer, this->output_buffer);
	} else {
		cvCopy(src, this->output_buffer);
	}
}

void moMaskModule::reloadMask() {
	if (this->mask_buffer != NULL) {
		cvReleaseImage(&(this->mask_buffer));
		this->mask_buffer = NULL;
	}

	this->mask_buffer = cvLoadImage(this->property("filename").asString().c_str());
	if ( this->mask_buffer == NULL ) {
		LOGM(MO_ERROR) << "could not load mask file: " << this->property("filename").asString();
		this->setError("unable to load mask file");
	}
	else {
		this->notifyUpdate();
	}

}

