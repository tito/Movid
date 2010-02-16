#ifndef OT_MODULE_H
#define OT_MODULE_H

#include <string>
#include <map>

#include "otProperty.h"

class otDataStream;

enum {
	OT_MODULE_NONE		= 0x00000000,
	OT_MODULE_INPUT		= 0x00000001,
	OT_MODULE_OUTPUT	= 0x00000002
};

class otModule {
public:	
	otModule(unsigned int capabilities, int input_count, int output_count);
	virtual ~otModule();
	
	virtual void setInput(otDataStream* stream, int n=0) = 0;
	virtual otDataStream* getOutput(int n=0) = 0;

	virtual int getInputCount();
	virtual int getOutputCount();
	virtual std::string getInputName(int n=0);
	virtual std::string getOutputName(int n=0);
	virtual std::string getInputType(int n=0);
	virtual std::string getOutputType(int n=0);

	virtual void notifyData(otDataStream *source);

	virtual void start();
	virtual void stop();
	virtual void update() = 0;
	virtual void lock();
	virtual void unlock();
	virtual bool isStarted();

	otProperty &property(std::string name);

	unsigned int getCapabilities();
	
	// FIXME protect it
	otModule *owner;

private:
	unsigned int capabilities;
	int	input_count;
	int	output_count;
	bool is_started;

protected:
	std::map<int, std::string> input_types;
	std::map<int, std::string> input_names;
	std::map<int, std::string> output_types;
	std::map<int, std::string> output_names;
	std::map<std::string, otProperty*> properties;
};

#endif

