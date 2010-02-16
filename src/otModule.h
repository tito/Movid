#ifndef OT_MODULE_H
#define OT_MODULE_H

#include <string>
#include <map>

#include "otLog.h"
#include "otProperty.h"

#define MODULE_DECLARE(name, author, description) \
	LOG_DECLARE(#name); \
	MODULE_DECLARE_EX(name, Module, author, description);

#define MODULE_DECLARE_EX(name, module, author, description) \
	static std::string module_name = #name; \
	static std::string module_author = author; \
	static std::string module_description = description; \
	std::string ot##name##module::getName() { return module_name; } \
	std::string ot##name##module::getDescription() { return module_description; } \
	std::string ot##name##module::getAuthor() { return module_author; }

#define MODULE_INIT() \
	this->properties["id"] = new otProperty(otModule::createId(module_name)); \
	LOG(DEBUG) << "create object <" << module_name << "> with id <" \
			   << this->property("id").asString() << ">";

#define MODULE_INTERNALS() 					\
	public:									\
	virtual std::string getName(); 			\
	virtual std::string getDescription(); 	\
	virtual std::string getAuthor(); 		\

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

	static std::string createId(std::string base);

	virtual std::string getName() = 0;
	virtual std::string getDescription() = 0;
	virtual std::string getAuthor() = 0;

	void describe();
	virtual bool isPipeline();
	
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

