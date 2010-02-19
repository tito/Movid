#include <assert.h>

#include "otDataStream.h"
#include "otModule.h"

otDataStream::otDataStream(std::string format) {
	this->format = format;
	this->data	 = NULL;
}

otDataStream::~otDataStream() {
}

std::string otDataStream::getFormat() {
	return this->format;
}

void otDataStream::setFormat(const std::string &format) {
	this->format = format;
}

void otDataStream::lock() {
	// FIXME
}

void otDataStream::unlock() {
	// FIXME
}

void otDataStream::push(void *data) {
	this->lock();
	this->data = data;
	this->unlock();

	this->notifyObservers();
}

void otDataStream::addObserver(otModule *module) {
	this->observers.push_back(module);
}

void otDataStream::removeObserver(otModule *module) {
	std::vector<otModule *>::iterator it;
	// FIXME lock needed ?
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		if ( *it != module )
			continue;
		this->observers.erase(it);
		return;
	}
}

void otDataStream::notifyObservers() {
	std::vector<otModule *>::iterator it;
	for ( it = this->observers.begin(); it != this->observers.end(); it++ )
		(*it)->notifyData(this);
}

void *otDataStream::getData() {
	return this->data;
}

unsigned int otDataStream::getObserverCount() {
	return this->observers.size();
}

otModule *otDataStream::getObserver(unsigned int index) {
	assert( index >= 0 && index < this->observers.size() );
	return this->observers[index];
}

void otDataStream::removeObservers() {
	std::vector<otModule *>::iterator it;
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		for ( int i = 0; i < (*it)->getInputCount(); i++ )
			if ( (*it)->getInput(i) == this )
				(*it)->setInput(NULL, i);
	}
	this->observers.clear();
}
