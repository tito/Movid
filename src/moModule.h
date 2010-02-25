#ifndef MO_MODULE_H
#define MO_MODULE_H

#include <string>
#include <map>

#include "moLog.h"
#include "moProperty.h"

#define MODULE_DECLARE(name, author, description) \
	LOG_DECLARE(#name); \
	MODULE_DECLARE_EX(name, Module, author, description);

#define MODULE_DECLARE_EX(name, module, author, description) \
	static std::string module_name = #name; \
	static std::string module_author = author; \
	static std::string module_description = description; \
	std::string mo##name##module::getName() { return module_name; } \
	std::string mo##name##module::getDescription() { return module_description; } \
	std::string mo##name##module::getAuthor() { return module_author; } \
	moModule *factory_create_mo##name() { return new mo##name##module(); }

#define MODULE_INIT() \
	this->properties["id"] = new moProperty(moModule::createId(module_name)); \
	LOG(DEBUG) << "create object <" << module_name << "> with id <" \
			   << this->property("id").asString() << ">";

#define MODULE_INTERNALS() 					\
	public:									\
	virtual std::string getName(); 			\
	virtual std::string getDescription(); 	\
	virtual std::string getAuthor();

class moDataStream;
class moDataStreamInfo;
class moPipeline;

enum {
	MO_MODULE_NONE		= 0x00000000,
	MO_MODULE_INPUT		= 0x00000001,
	MO_MODULE_OUTPUT	= 0x00000002
};

class moModule {
public:	
	moModule(unsigned int capabilities, int input_count, int output_count);
	virtual ~moModule();
	
	virtual void setInput(moDataStream* stream, int n=0) = 0;
	virtual moDataStream *getInput(int n=0) = 0;
	virtual moDataStream *getOutput(int n=0) = 0;

	virtual int getInputCount();
	virtual int getOutputCount();
	virtual moDataStreamInfo *getInputInfos(int n=0);
	virtual moDataStreamInfo *getOutputInfos(int n=0);

	virtual void notifyData(moDataStream *source);

	virtual void start();
	virtual void stop();
	virtual void update() = 0;
	virtual void lock();
	virtual void unlock();
	virtual bool isStarted();

	moProperty &property(std::string name);
	std::map<std::string, moProperty*> &getProperties();

	unsigned int getCapabilities();

	static std::string createId(std::string base);

	virtual std::string getName() = 0;
	virtual std::string getDescription() = 0;
	virtual std::string getAuthor() = 0;

	void describe();
	virtual bool isPipeline();
	
private:
	unsigned int capabilities;
	bool is_started;

protected:
	moModule *owner;
	int	input_count;
	int	output_count;
	std::map<int, moDataStreamInfo*> input_infos;
	std::map<int, moDataStreamInfo*> output_infos;

	std::map<std::string, moProperty*> properties;

	friend class moPipeline;
};

#endif

