#include <assert.h>
#include <sstream>
#include <iostream>

#include "otModule.h"
#include "otDataStream.h"

LOG_DECLARE("Module");

static unsigned int idcount = 0;

otModule::otModule(unsigned int capabilities, int input_count, int output_count) {
	this->capabilities	= capabilities;
	this->input_count	= input_count;
	this->output_count	= output_count;
	this->is_started	= false;
	this->owner			= NULL;
}

otModule::~otModule() {
	this->stop();

	if ( this->properties.size() > 0 ) {
		std::map<std::string, otProperty*>::iterator it;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
			delete (*it).second;
			(*it).second = NULL;
		}
	}
}

std::string otModule::createId(std::string base) {
	std::ostringstream oss;
	oss << base << (idcount++);
	return oss.str();
}

unsigned int otModule::getCapabilities() {
	return this->capabilities;
}

int otModule::getInputCount() {
	return this->input_count;
}

int otModule::getOutputCount() {
	return this->output_count;
}

void otModule::notifyData(otDataStream *source) {
}

void otModule::start() {
	this->is_started = true;
	LOG(DEBUG) << "start <" << this->property("id").asString() << ">";
}

void otModule::stop() {
	this->is_started = false;
	LOG(DEBUG) << "stop <" << this->property("id").asString() << ">";
}

void otModule::lock() {
	// implement if a thread.
}

void otModule::unlock() {
	// implement if a thread.
}

bool otModule::isStarted() {
	return this->is_started;
}

std::string otModule::getInputName(int n) {
	std::map<int, std::string>::iterator it;
	it = this->input_names.find(n);
	if ( it == this->input_names.end() )
		return "unamed";
	return it->second;
}

std::string otModule::getOutputName(int n) {
	std::map<int, std::string>::iterator it;
	it = this->output_names.find(n);
	if ( it == this->output_names.end() )
		return "unamed";
	return it->second;
}

std::string otModule::getInputType(int n) {
	std::map<int, std::string>::iterator it;
	it = this->input_types.find(n);
	if ( it == this->input_types.end() )
		return "unamed";
	return it->second;
}

std::string otModule::getOutputType(int n) {
	std::map<int, std::string>::iterator it;
	it = this->output_types.find(n);
	if ( it == this->output_types.end() )
		return "unamed";
	return it->second;
}

otProperty &otModule::property(std::string str) {
	static otProperty invalid(0);
	std::map<std::string, otProperty*>::iterator it;
	it = this->properties.find(str);
	if ( it == this->properties.end() )
		return invalid;
	return *it->second;
}

void otModule::describe() {
	std::cout << "Module: " << this->getName() << std::endl;
	std::cout << "Author: " << this->getAuthor() << std::endl;
	std::cout << "Description: " << this->getDescription() << std::endl;

	std::cout << "Capabilities: ";
	if ( this->getCapabilities() & OT_MODULE_INPUT )
		std::cout << "input,";
	if ( this->getCapabilities() & OT_MODULE_OUTPUT )
		std::cout << "output,";
	std::cout << std::endl;

	if ( this->properties.size() > 0 ) {
		std::cout << std::endl;
		std::cout << "Properties: " << std::endl;

		std::map<std::string, otProperty*>::iterator it;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
			std::cout << " " << (*it).first << ": " \
				<< "type=" << otProperty::getPropertyTypeName((*it).second->getType()) << ", "\
				<< "default=" << (*it).second->asString() \
				<< std::endl;
		}
	}

	if ( this->getCapabilities() & OT_MODULE_INPUT ) {
		std::cout << std::endl;
		std::cout << "Input :" << std::endl;
		for ( int i = 0; i < this->getInputCount(); i++ ) {
			std::cout << " " << i << ": name=" \
				<< this->getInputName(i) << ", type=" \
				<< this->getInputType(i) << std::endl;
		}
	}

	if ( this->getCapabilities() & OT_MODULE_OUTPUT ) {
		std::cout << std::endl;
		std::cout << "Output :" << std::endl;
		for ( int i = 0; i < this->getOutputCount(); i++ ) {
			std::cout << " " << i << ": name=" \
				<< this->getOutputName(i) << ", type=" \
				<< this->getOutputType(i) << std::endl;
		}
	}

	std::cout << std::endl;
}

