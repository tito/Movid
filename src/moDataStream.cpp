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

#include "moDataStream.h"
#include "moModule.h"
#include "moUtils.h"

bool moDataStreamInfo::isStreamValid(moDataStream *stream) {
	if ( stream == NULL )
		return false;
	return moUtils::inList(stream->getFormat(), this->type, ",");
}

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

restartremove:;
	for ( it = this->observers.begin(); it != this->observers.end(); it++ ) {
		for ( int i = 0; i < (*it)->getInputCount(); i++ ) {
			if ( (*it)->getInput(i) == this ) {
				(*it)->setInput(NULL, i);
				// the list have been altered, must restart from scratch
				goto restartremove;
			}
		}
	}

	this->observers.clear();
}
