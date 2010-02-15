#ifndef OT_MODULE_H
#define OT_MODULE_H

#include <string>
#include <map>

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
	int getInputCount();
	int getOutputCount();

	virtual std::string getInputName(int n=0);
	virtual std::string getOutputName(int n=0);
	std::string getInputType(int n=0);
	std::string getOutputType(int n=0);

	virtual void notifyData(otDataStream *source);
	virtual void update();

	virtual void start();
	virtual void stop();
	virtual bool isStarted();
	virtual void lock();
	virtual void unlock();

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
};

#endif

