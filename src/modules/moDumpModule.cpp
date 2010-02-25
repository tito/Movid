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

