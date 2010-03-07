#ifndef MO_DATA_STREAM_H
#define MO_DATA_STREAM_H

#include <string>
#include <vector>

#include "pasync.h"

class moModule;

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

