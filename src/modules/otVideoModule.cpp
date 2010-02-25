#include "highgui.h"

#include "otVideoModule.h"
#include "../otDataStream.h"
#include "../otLog.h"

MODULE_DECLARE(Video, "native", "Provide a stream from a video file");

otVideoModule::otVideoModule() : otModule(OT_MODULE_OUTPUT, 0, 1) {

	MODULE_INIT();

	this->video = NULL;
	this->stream = new otDataStream("IplImage");

	// declare outputs
	this->output_infos[0] = new otDataStreamInfo("video", "IplImage", "Video image stream");

	// declare properties
	this->properties["filename"] = new otProperty("");
	this->properties["loop"] = new otProperty(true);
}

otVideoModule::~otVideoModule() {
}

void otVideoModule::start() {
	assert( this->video == NULL );
	otModule::start();
	LOGM(TRACE) << "start video";
	this->video = cvCaptureFromAVI(this->property("filename").asString().c_str());
	this->numframes = (int)cvGetCaptureProperty(static_cast<CvCapture *>(this->video), CV_CAP_PROP_FRAME_COUNT);
}

void otVideoModule::stop() {
	if ( this->video != NULL ) {
		LOGM(TRACE) << "release video";
		cvReleaseCapture((CvCapture **)&this->video);
		this->video = NULL;
	}
	otModule::stop();
}

void otVideoModule::update() {
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

void otVideoModule::setInput(otDataStream* input, int n) {
	assert( "no input supported on otVideoModule()" && 0 );
}

otDataStream* otVideoModule::getInput(int n) {
	return NULL;
}

otDataStream* otVideoModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

