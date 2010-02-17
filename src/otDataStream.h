#ifndef OT_DATA_STREAM_H
#define OT_DATA_STREAM_H

#include <string>
#include <vector>

class otModule;

class otDataStream {
	
public:
	otDataStream(std::string format); 
	virtual ~otDataStream();
	
	void addObserver(otModule *module);
	void removeObserver(otModule *module);
	unsigned int getObserverCount();
	otModule *getObserver(unsigned int index);

	void push(void *data);
	void *getData();

	void lock();
	void unlock();

	std::string getFormat();
	
protected:
	std::string format;
	void *data;
	std::vector<otModule*> observers;

	void notifyObservers();
};

#endif

