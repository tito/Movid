#include "highgui.h"

#include "otVideoModule.h"
#include "../otDataStream.h"
#include "../otLog.h"

LOG_DECLARE("Video");
MODULE_DECLARE(Video, "native", "Provide a stream from a video file");

otVideoModule::otVideoModule() : otModule(OT_MODULE_OUTPUT, 0, 1) {

	LOG(DEBUG) << "Create otVideoModule";

	this->video = NULL;
	this->stream = new otDataStream("IplImage");

	// declare outputs
	this->output_names[0] = std::string("video");
	this->output_types[0] = std::string("IplImage");

	// declare properties
	this->properties["filename"] = new otProperty("");
}

otVideoModule::~otVideoModule() {
	this->stop();
}

void otVideoModule::start() {
	assert( this->video == NULL );
	otModule::start();
	LOG(INFO) << "Video playing started";

	this->video = cvCaptureFromAVI(this->property("filename").asString().c_str());
}

void otVideoModule::stop() {
	if ( this->video != NULL ) {
		LOG(DEBUG) << "Release video";
		cvReleaseCapture((CvCapture **)&this->video);
		this->video = NULL;
	}
}

void otVideoModule::update() {
	// push a new image on the stream
	LOG(DEBUG) << "Push a new image on the stream";
	this->stream->push(cvQueryFrame(static_cast<CvCapture *>(this->video)));
}

void otVideoModule::setInput(otDataStream* input, int n) {
	assert( "no input supported on otVideoModule()" && 0 );
}

otDataStream* otVideoModule::getOutput(int n) {
	assert( n == 0 );
	return this->stream;
}

