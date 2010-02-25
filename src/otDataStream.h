#ifndef OT_DATA_STREAM_H
#define OT_DATA_STREAM_H

#include <string>
#include <vector>

class otModule;

class otDataStreamInfo {
public:
	otDataStreamInfo(const std::string &name,
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

class otDataStream {
	
public:
	otDataStream(std::string format); 
	virtual ~otDataStream();
	
	void addObserver(otModule *module);
	void removeObserver(otModule *module);
	void removeObservers();
	unsigned int getObserverCount();
	otModule *getObserver(unsigned int index);

	void push(void *data);
	void *getData();

	void lock();
	void unlock();

	std::string getFormat();
	void setFormat(const std::string& format);
	
protected:
	std::string format;
	void *data;
	std::vector<otModule*> observers;

	void notifyObservers();
};

#endif

