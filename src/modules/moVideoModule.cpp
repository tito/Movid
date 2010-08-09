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


#include "highgui.h"

#include "moVideoModule.h"
#include "../moDataStream.h"
#include "../moLog.h"

MODULE_DECLARE(Video, "native", "Provide a stream from a video file");

moVideoModule::moVideoModule() : moModule(MO_MODULE_OUTPUT) {

	MODULE_INIT();

	this->video = NULL;
	this->stream = new moDataStream("IplImage");

	// declare outputs
	this->declareOutput(0, &this->stream, new moDataStreamInfo(
			"video", "IplImage", "Video image stream"));

	// declare properties
	this->properties["filename"] = new moProperty("");
	this->properties["loop"] = new moProperty(true);
}

moVideoModule::~moVideoModule() {
	if ( this->stream != NULL )
		delete this->stream;
}

void moVideoModule::start() {
	assert( this->video == NULL );
	LOGM(MO_TRACE, "start video");
	this->video = cvCaptureFromAVI(this->property("filename").asString().c_str());
	this->numframes = (int)cvGetCaptureProperty(static_cast<CvCapture *>(this->video), CV_CAP_PROP_FRAME_COUNT);
	moModule::start();
}

void moVideoModule::stop() {
	moModule::stop();
	if ( this->video != NULL ) {
		LOGM(MO_TRACE, "release video");
		cvReleaseCapture((CvCapture **)&this->video);
		this->video = NULL;
	}
}

void moVideoModule::update() {
	// push a new image on the stream
	LOGM(MO_TRACE, "push a new image on the stream");
	this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->video)));

	if ( this->numframes-- > 0 )
		return;

	if ( this->property("loop").asBool() ) {
		cvSetCaptureProperty(static_cast<CvCapture *>(this->video), CV_CAP_PROP_POS_FRAMES, 0);
		this->numframes = (int)cvGetCaptureProperty(static_cast<CvCapture *>(this->video), CV_CAP_PROP_FRAME_COUNT);
	}

}

void moVideoModule::poll() {
	this->notifyUpdate();
	moModule::poll();
}

