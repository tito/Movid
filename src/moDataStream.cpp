#include <assert.h>

#include "moDataStream.h"
#include "moModule.h"

moDataStream::moDataStream(std::string format) {
	this->format = format;
	this->data	 = NULL;
	this->mtx	 = new pt::mutex();
}

moDataStream::~moDataStream() {
	delete this->mtx;
}

std::string moDataStream::getFormat() {
	return this->format;
}

void moDataStream::setFormat(const std::string &format) {
	this->format = format;
}

void moDataStream::lock() {
	this->mtx->lock();
}

void moDataStream::unlock() {
	this->mtx->unlock();
}

void moDataStream::push(void *data) {
	this->lock();
	this->data = data;
	this->unlock();

	this->notifyObservers();
}

void moDataStream::addObserver(moModule *module) {
	this->observers.push_back(module);
}

void moDataStream::removeObserver(moModule *module) {
	std::vector<moModule *>::iterator it;
	// FIXME lock needed ?
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		if ( *it != module )
			continue;
		this->observers.erase(it);
		return;
	}
}

void moDataStream::notifyObservers() {
	std::vector<moModule *>::iterator it;
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		(*it)->lock();
		(*it)->notifyData(this);
		(*it)->unlock();
	}
}

void *moDataStream::getData() {
	return this->data;
}

unsigned int moDataStream::getObserverCount() {
	return this->observers.size();
}

moModule *moDataStream::getObserver(unsigned int index) {
	assert( index >= 0 && index < this->observers.size() );
	return this->observers[index];
}

void moDataStream::removeObservers() {
	std::vector<moModule *>::iterator it;
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		for ( int i = 0; i < (*it)->getInputCount(); i++ )
			if ( (*it)->getInput(i) == this )
				(*it)->setInput(NULL, i);
	}
	this->observers.clear();
}
