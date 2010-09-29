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

#include "../moLog.h"
#include "../moModule.h"
#include "../moDataStream.h"
#include "moCameraModule.h"
#include "highgui.h"

MODULE_DECLARE(Camera, "native", "Fetch camera stream");

moCameraModule::moCameraModule() : moModule(MO_MODULE_OUTPUT) {

	MODULE_INIT();

	this->camera = NULL;
	this->stream = new moDataStream("IplImage");
	this->monochrome = new moDataStream("IplImage8");

	// declare outputs
	this->declareOutput(0, &this->stream, new moDataStreamInfo(
			"camera", "IplImage", "Image stream of the camera"));
	this->declareOutput(1, &this->monochrome, new moDataStreamInfo(
			"camera", "IplImage8", "Image stream of the camera"));

	// declare properties
	this->properties["index"] = new moProperty(0);
}

moCameraModule::~moCameraModule() {
}

void moCameraModule::start() {
	assert( this->camera == NULL );
	LOGM(MO_TRACE, "start camera");

	this->camera = cvCaptureFromCAM(this->property("index").asInteger());
	if ( this->camera == NULL ) {
		LOGM(MO_ERROR, "could not load camera: " << this->property("index").asInteger());
		this->setError("Unable to open camera");
	}
	moModule::start();
}

void moCameraModule::stop() {
	moModule::stop();
	if ( this->camera != NULL ) {
		LOGM(MO_TRACE, "release camera");
		cvReleaseCapture((CvCapture **)&this->camera);
		this->camera = NULL;
	}
}

void moCameraModule::update() {
	if ( this->camera != NULL ) {
		// push a new image on the stream
		LOGM(MO_TRACE, "push a new image on the stream");
		IplImage *img = cvQueryFrame(static_cast<CvCapture *>(this->camera));
        if (img != NULL) {
            if (img->nChannels == 3)
                this->stream->push(img);
            else if (img->nChannels == 1)
                this->monochrome->push(img);
            else {
                std::cout << "CAMERA: Unsupported amount of camera image channels " << img->nChannels << std::endl;
                assert(0);
            }
            this->notifyUpdate();
        }
	}
}

void moCameraModule::poll() {
	this->notifyUpdate();
	moModule::poll();
}

