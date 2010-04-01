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


#include "moDumpModule.h"
#include "../moDataStream.h"
#include "../moDataGenericContainer.h"
#include "../moLog.h"
#include "../moModule.h"
#include "cv.h"

MODULE_DECLARE(Dump, "native", "Dump all data informations in console");

moDumpModule::moDumpModule() : moModule(MO_MODULE_INPUT, 1, 0) {
	this->stream = NULL;
	this->input_infos[0] = new moDataStreamInfo("data", "*", "Show any data input in text format");
}

moDumpModule::~moDumpModule() {
}

void moDumpModule::setInput(moDataStream *stream, int n) {
	if ( this->stream != NULL )
		this->stream->removeObserver(this);
	this->stream = stream;
	if ( this->stream != NULL )
		this->stream->addObserver(this);
}

moDataStream *moDumpModule::getInput(int n) {
	return this->stream;
}

moDataStream *moDumpModule::getOutput(int n) {
	return NULL;
}

void moDumpModule::update() {
}

void moDumpModule::notifyData(moDataStream *stream) {
	LOG(MO_INFO) << "stream<" << stream << ">, type=" << stream->getFormat() << ", observers=" << stream->getObserverCount();
	if ( stream->getFormat() == "IplImage" ) {
		IplImage *img = static_cast<IplImage *>(stream->getData());
		LOG(MO_INFO) << " `- Image size=" << img->width << "x" << img->height \
			<< ", channels=" << img->nChannels \
			<< ", depth=" << img->depth;
		return;
	}

	if ( stream->getFormat() == "touch" ||
		 stream->getFormat() == "fiducial" ) {
		moDataGenericList *list = static_cast<moDataGenericList*>(stream->getData());
		LOG(MO_INFO) << " `- " << stream->getFormat() << " size=" << list->size();
	}
}

