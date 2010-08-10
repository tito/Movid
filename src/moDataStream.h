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


#ifndef MO_DATA_STREAM_H
#define MO_DATA_STREAM_H

#include <string>
#include <vector>

#include "pasync.h"

class moModule;
class moDataStream;

class moDataStreamInfo {
public:
	moDataStreamInfo(const std::string &name,
					 const std::string &type,
					 const std::string &description = "") {
		this->name = name;
		this->type = type;
		this->description = description;
	};

	std::string getName() {
		return this->name;
	}

	std::string getType() {
		return this->type;
	}

	std::string getDescription() {
		return this->description;
	}

	void setType(const std::string &type) {
		this->type = type;
	}

	bool isStreamValid(moDataStream *stream);

protected:
	std::string name;
	std::string type;
	std::string description;
};

class moDataStream {
	
public:
	moDataStream(std::string format); 
	virtual ~moDataStream();
	
	void addObserver(moModule *module);
	void removeObserver(moModule *module);
	void removeObservers();
	unsigned int getObserverCount();
	moModule *getObserver(unsigned int index);

	void push(void *data);
	void *getData();

	void lock();
	void unlock();

	std::string getFormat();
	void setFormat(const std::string& format);
	
protected:
	std::string format;
	void *data;
	std::vector<moModule*> observers;
	pt::mutex *mtx;

	void notifyObservers();
};

#endif

