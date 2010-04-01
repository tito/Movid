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


#include <sstream>
#include <assert.h>

#include "cv.h"
#include "highgui.h"

#include "moImageDisplayModule.h"
#include "../moDataStream.h"
#include "../moLog.h"

MODULE_DECLARE(ImageDisplay, "native", "Display image on a window");

static unsigned int count = 0;

moImageDisplayModule::moImageDisplayModule() : moModule(MO_MODULE_INPUT, 1, 0) {

	MODULE_INIT();

	this->input = NULL;
	this->img = NULL;

	// declare inputs
	this->input_infos[0] = new moDataStreamInfo(
			"image", "IplImage", "Show image stream in a window");

	// declare properties
	std::ostringstream oss;
	oss << "Movid" << (count++);
	this->properties["name"] = new moProperty(oss.str());
}

moImageDisplayModule::~moImageDisplayModule(){
	if ( this->img != NULL )
		cvReleaseImage(&this->img);
}

void moImageDisplayModule::stop() {
	moModule::stop();
	cvDestroyWindow(this->property("name").asString().c_str());
}

void moImageDisplayModule::notifyData(moDataStream *input) {
	// ensure that input data is IfiImage
	assert( input != NULL );
	assert( input == this->input );
	assert( input->getFormat() == "IplImage" );


	// out input have been updated !
	this->input->lock();
	if ( this->img != NULL ) {
		cvReleaseImage(&this->img);
		this->img = NULL;
	}
	if ( this->input->getData() != NULL )
		this->img = cvCloneImage(static_cast<IplImage*>(this->input->getData()));
	this->input->unlock();

	this->notifyUpdate();
}

void moImageDisplayModule::setInput(moDataStream *stream, int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return;
	}

	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( stream != NULL ) {
		if ( stream->getFormat() != "IplImage" ) {
			this->setError("Input 0 accept only IplImage");
			this->input = NULL;
			return;
		}
	}
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moImageDisplayModule::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moImageDisplayModule::getOutput(int n) {
	this->setError("no output supported");
	return NULL;
}

void moImageDisplayModule::update() {
	this->input->lock();
	cvShowImage(this->property("name").asString().c_str(), this->img);
	this->input->unlock();
}

