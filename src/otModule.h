#ifndef OT_MODULE_H
#define OT_MODULE_H

class otDataStream;

class otModule {
public:	
	virtual ~otModule();
	
	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream* getOutput(int n=0);

	virtual void update(otDataStream *source);

	virtual void start();
	virtual void stop();
	virtual bool isStarted();
	virtual void lock();
	virtual void unlock();
	
	// FIXME protect it
	otModule *owner;

private:
	bool is_started;
	otDataStream *input;
};

#endif

