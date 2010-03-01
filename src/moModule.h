#ifndef MO_MODULE_H
#define MO_MODULE_H

#include <string>
#include <map>

#include "moLog.h"
#include "moProperty.h"

class moDataStream;
class moDataStreamInfo;
class moPipeline;


/*! \brief Declare a new module (create all needed informations) + Log declaration
 *
 * \param name Name of the module
 * \param author Author of the module (native mean not external)
 * \param description Description of the module
 */
#define MODULE_DECLARE(name, author, description) \
	LOG_DECLARE(#name); \
	MODULE_DECLARE_EX(name, Module, author, description);

/*! \brief Declare a new module (create all needed informations)
 *
 * \param name Name of the module
 * \param module Postfix of the classname module
 * \param author Author of the module (native mean not external)
 * \param description Description of the module
 */
#define MODULE_DECLARE_EX(name, module, author, description) \
	static std::string module_name = #name; \
	static std::string module_author = author; \
	static std::string module_description = description; \
	std::string mo##name##module::getName() { return module_name; } \
	std::string mo##name##module::getDescription() { return module_description; } \
	std::string mo##name##module::getAuthor() { return module_author; } \
	moModule *factory_create_mo##name() { return new mo##name##module(); }


/*! \brief Call initialization of the module (must be inserted in constructor)
 */
#define MODULE_INIT() \
	this->properties["id"] = new moProperty(moModule::createId(module_name)); \
	LOG(DEBUG) << "create object <" << module_name << "> with id <" \
			   << this->property("id").asString() << ">";

/*! \brief Declare needed functions for a module (must be inserted at end of a module)
 */
#define MODULE_INTERNALS() 					\
	public:									\
	virtual std::string getName(); 			\
	virtual std::string getDescription(); 	\
	virtual std::string getAuthor();

enum {
	MO_MODULE_NONE		= 0x00000000,		/*< Module have no input/output */
	MO_MODULE_INPUT		= 0x00000001,		/*< Module have inputs */
	MO_MODULE_OUTPUT	= 0x00000002		/*< Module have outputs */
};

/*! \brief Base class for all modules
 */
class moModule {
public:	

	/*! \brief Instance a module
	 *
	 * \param capabilities combine flags from MO_MODULE_*
	 * \param input_count number of inputs
	 * \param output_count number of outputs
	 *
	 * \return an instance of moModule()
	 */
	moModule(unsigned int capabilities, int input_count, int output_count);

	/*! \brief Free the module
	 */
	virtual ~moModule();
	
	/*! \brief Connect an input from an existing moDataStream
	 *
	 * \param stream instance of a moDataStream()
	 * \param n index of the input to connect
	 */
	virtual void setInput(moDataStream* stream, int n=0) = 0;

	/*! \brief Get a input data stream
	 *
	 * \param n index of the input to get
	 *
	 * \return the selected input.
	 */
	virtual moDataStream *getInput(int n=0) = 0;

	/*! \brief Get a output data stream
	 *
	 * \param n index of the output to get
	 *
	 * \return the selected output.
	 */
	virtual moDataStream *getOutput(int n=0) = 0;

	/*! \brief Get number of input stream
	 */
	virtual int getInputCount();

	/*! \brief Get number of output stream
	 */
	virtual int getOutputCount();

	/*! \brief Get informations about input stream
	 *
	 * \param n index of the input to get
	 *
	 * \return an instance of moDataStreamInfo()
	 */
	virtual moDataStreamInfo *getInputInfos(int n=0);

	/*! \brief Get informations about output stream
	 *
	 * \param n index of the output to get
	 *
	 * \return an instance of moDataStreamInfo()
	 */
	virtual moDataStreamInfo *getOutputInfos(int n=0);

	/*! \brief Start the module
	 */
	virtual void start();

	/*! \brief Stop the module
	 */
	virtual void stop();

	/*! \brief Update the module
	 */
	virtual void update() = 0;

	/*! \brief Lock access to the module data
	 */
	virtual void lock();

	/*! \brief Unlock access to the module data
	 */
	virtual void unlock();

	/*! \brief Check if the module is started
	 *
	 * \return true is the module is started, otherwise false
	 */
	virtual bool isStarted();

	/*! \brief Get a property from the module
	 * If the property not exist, it will be automaticly created
	 *
	 * \param name name of the property to lookup
	 *
	 * \return return the reference of the property
	 */
	moProperty &property(std::string name);

	/*! \brief Get all properties
	 */
	std::map<std::string, moProperty*> &getProperties();

	/*! \brief Get capabilities flags
	 */
	unsigned int getCapabilities();

	/*! \brief Get the name of the module
	 */
	virtual std::string getName() = 0;

	/*! \brief Get the description of the module
	 */
	virtual std::string getDescription() = 0;

	/*! \brief Get the author of the module
	 */
	virtual std::string getAuthor() = 0;

	/*! \brief Show on console all information on the module
	 */
	void describe();

	
private:
	/*! \brief Capabilities flags
	 */
	unsigned int capabilities;

	/*! \brief Boolean to store if the module is started or not
	 */
	bool is_started;

protected:

	/*! \brief Pipeline that own the module
	 */
	moModule *owner;

	/*! \brief Number of input
	 */
	int	input_count;

	/*! \brief Number of output
	 */
	int	output_count;

	/*! \brief Input informations
	 */
	std::map<int, moDataStreamInfo*> input_infos;

	/*! \brief Output informations
	 */
	std::map<int, moDataStreamInfo*> output_infos;

	/*! \brief Properties storage
	 */
	std::map<std::string, moProperty*> properties;

	/*! \brief Indicate if the module is a pipeline
	 *
	 * \return true if it's a pipeline, otherwise false
	 */
	virtual bool isPipeline();

	/*! \brief Notify the module that an input data stream have been updated
	 *
	 * \param source instance of the updated data stream
	 */
	virtual void notifyData(moDataStream *source);

	/*! \brief Create a uniq id for the instance of the class
	 *
	 * \param base name of the class to use it
	 */
	static std::string createId(std::string base);


	friend class moDataStream;
	friend class moPipeline;
};

#endif

