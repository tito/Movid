#include "highgui.h"

#include "moVideoModule.h"
#include "../moDataStream.h"
#include "../moLog.h"

MODULE_DECLARE(Video, "native", "Provide a stream from a video file");

moVideoModule::moVideoModule() : moModule(MO_MODULE_OUTPUT, 0, 1) {

	MODULE_INIT();

	this->video = NULL;
	this->stream = new moDataStream("IplImage");

	// declare outputs
	this->output_infos[0] = new moDataStreamInfo("video", "IplImage", "Video image stream");

	// declare properties
	this->properties["filename"] = new moProperty("");
	this->properties["loop"] = new moProperty(true);
}

moVideoModule::~moVideoModule() {
}

void moVideoModule::start() {
	assert( this->video == NULL );
	moModule::start();
	LOGM(TRACE) << "start video";
	this->video = cvCaptureFromAVI(this->property("filename").asString().c_str());
	this->numframes = (int)cvGetCaptureProperty(static_cast<CvCapture *>(this->video), CV_CAP_PROP_FRAME_COUNT);
}

void moVideoModule::stop() {
	if ( this->video != NULL ) {
		LOGM(TRACE) << "release video";
		cvReleaseCapture((CvCapture **)&this->video);
		this->video = NULL;
	}
	moModule::stop();
}

void moVideoModule::update() {
	// push a new image on the stream
	LOGM(TRACE) << "push a new image on the stream";
	this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->video)));

	if ( this->numframes-- > 0 )
		return;

	if ( this->property("loop").asBool() ) {
		cvSetCaptureProperty(static_cast<CvCapture *>(this->video), CV_CAP_PROP_POS_FRAMES, 0);
		this->numframes = (int)cvGetCaptureProperty(static_cast<CvCapture *>(this->video), CV_CAP_PROP_FRAME_COUNT);
	}

}

void moVideoModule::setInput(moDataStream* input, int n) {
	assert( "no input supported on moVideoModule()" && 0 );
}

moDataStream* moVideoModule::getInput(int n) {
	return NULL;
}

moDataStream* moVideoModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

