#include "otDumpModule.h"
#include "../otDataStream.h"
#include "../otDataGenericContainer.h"
#include "../otLog.h"
#include "../otModule.h"
#include "cv.h"

MODULE_DECLARE(Dump, "native", "Dump all data informations in console");

otDumpModule::otDumpModule() : otModule(OT_MODULE_INPUT, 1, 0) {
	this->stream = NULL;
}

otDumpModule::~otDumpModule() {
}

void otDumpModule::setInput(otDataStream *stream, int n) {
	if ( this->stream != NULL )
		this->stream->removeObserver(this);
	this->stream = stream;
	if ( this->stream != NULL )
		this->stream->addObserver(this);
}

otDataStream *otDumpModule::getInput(int n) {
	return this->stream;
}

otDataStream *otDumpModule::getOutput(int n) {
	return NULL;
}

void otDumpModule::update() {
}

void otDumpModule::notifyData(otDataStream *stream) {
	LOG(INFO) << "stream<" << stream << ">, type=" << stream->getFormat() << ", observers=" << stream->getObserverCount();
	if ( stream->getFormat() == "IplImage" ) {
		IplImage *img = static_cast<IplImage *>(stream->getData());
		LOG(INFO) << " `- Image size=" << img->width << "x" << img->height \
			<< ", channels=" << img->nChannels \
			<< ", depth=" << img->depth;
		return;
	}

	if ( stream->getFormat() == "otDataGenericList" ) {
		otDataGenericList *list = static_cast<otDataGenericList*>(stream->getData());
		LOG(INFO) << " `- DataGenericList size=" << list->size();
	}
}

