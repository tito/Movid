#ifndef OT_DATA_STREAM_H
#define OT_DATA_STREAM_H

#include <vector>

class otModule;


class otDataStream {
	
public:
	otDataStream(); 
	virtual ~otDataStream();
	
	virtual void update();
	void addObserver(otModule*);
	
	
protected:
	std::vector<otModule*> observers;

	void notifyObservers();
	
};

#endif