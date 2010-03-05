#include <assert.h>
#include <sstream>
#include <iostream>
#include <errno.h>

#include "moModule.h"
#include "moDataStream.h"
#include "moLog.h"
#include "moThread.h"

LOG_DECLARE("Module");

static unsigned int idcount = 0;

moModule::moModule(unsigned int capabilities, int input_count, int output_count) {
	this->capabilities	= capabilities;
	this->input_count	= input_count;
	this->output_count	= output_count;
	this->is_started	= false;
	this->owner			= NULL;
	this->is_error		= false;
	this->error_msg		= "";
	this->thread		= NULL;
	this->use_thread	= false;
	this->need_update	= false;

	this->properties["use_thread"] = new moProperty(false);
}

moModule::~moModule() {
	this->stop();

	if ( this->output_infos.size() > 0 ) {
		std::map<int, moDataStreamInfo*>::iterator it;
		for ( it = this->output_infos.begin(); it != this->output_infos.end(); it++ )
			delete it->second;
	}

	if ( this->input_infos.size() > 0 ) {
		std::map<int, moDataStreamInfo*>::iterator it;
		for ( it = this->input_infos.begin(); it != this->input_infos.end(); it++ )
			delete it->second;
	}

	if ( this->properties.size() > 0 ) {
		std::map<std::string, moProperty*>::iterator it;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
			delete (*it).second;
			(*it).second = NULL;
		}
	}
}

std::string moModule::createId(std::string base) {
	std::ostringstream oss;
	oss << base << (idcount++);
	return oss.str();
}

unsigned int moModule::getCapabilities() {
	return this->capabilities;
}

int moModule::getInputCount() {
	return this->input_count;
}

int moModule::getOutputCount() {
	return this->output_count;
}

void moModule::notifyData(moDataStream *source) {
}

void _thread_process(moThread *thread) {
	moModule *module = (moModule *)thread->getUserData();
	while ( !thread->wantQuit() ) {
		module->needUpdate(true);
		module->update();
	}
}

void moModule::start() {
	this->use_thread = this->property("use_thread").asBool();
	if ( this->use_thread ) {
		LOGM(TRACE) << "create semaphore";

		if ( sem_init(&this->sem_need_update, 0, 0) != 0 ) {
			LOGM(ERROR) << "unable to init semaphore";
			this->setError("unable to init semaphore");
			this->use_thread = false;
		}

		LOGM(TRACE) << "start thread";
		this->thread = new moThread(_thread_process, this);
		if ( this->thread == NULL ) {
			LOGM(ERROR) << "unable to create thread";
			this->setError("Error while creating thread");
			this->use_thread = false;
		} else {
			this->thread->start();
		}
	}

	this->is_started = true;
	LOGM(DEBUG) << "start";
}

void moModule::stop() {
	if ( this->use_thread ) {
		if ( this->thread != NULL ) {
			this->thread->cleanup();
			delete this->thread;

			sem_destroy(&this->sem_need_update);
		}
	}

	this->is_started = false;
	LOG(DEBUG) << "stop <" << this->property("id").asString() << ">";
}

void moModule::lock() {
	// implement if a thread.
}

void moModule::unlock() {
	// implement if a thread.
}

bool moModule::isStarted() {
	return this->is_started;
}

moDataStreamInfo *moModule::getInputInfos(int n) {
	std::map<int, moDataStreamInfo*>::iterator it;
	it = this->input_infos.find(n);
	if ( it == this->input_infos.end() )
		return NULL;
	return it->second;
}

moDataStreamInfo *moModule::getOutputInfos(int n) {
	std::map<int, moDataStreamInfo*>::iterator it;
	it = this->output_infos.find(n);
	if ( it == this->output_infos.end() )
		return NULL;
	return it->second;
}

moProperty &moModule::property(std::string str) {
	std::map<std::string, moProperty*>::iterator it;
	it = this->properties.find(str);
	if ( it == this->properties.end() ) {
		this->properties[str] = new moProperty("", "?? auto created ??");
		return *(this->properties[str]);
	}
	return *it->second;
}

std::map<std::string, moProperty*> &moModule::getProperties() {
	return this->properties;
}

void moModule::describe() {
	std::cout << "Module: " << this->getName() << std::endl;
	std::cout << "Author: " << this->getAuthor() << std::endl;
	std::cout << "Description: " << this->getDescription() << std::endl;

	std::cout << "Capabilities: ";
	if ( this->getCapabilities() & MO_MODULE_INPUT )
		std::cout << "input,";
	if ( this->getCapabilities() & MO_MODULE_OUTPUT )
		std::cout << "output,";
	std::cout << std::endl;

	if ( this->properties.size() > 0 ) {
		std::cout << std::endl;
		std::cout << "Properties: " << std::endl;

		std::map<std::string, moProperty*>::iterator it;
		for ( it = this->properties.begin(); it != this->properties.end(); it++ ) {
			std::cout << " " << (*it).first << ": " \
				<< "type=" << moProperty::getPropertyTypeName((*it).second->getType()) << ", "\
				<< "default=" << (*it).second->asString() \
				<< std::endl;
		}
	}

	if ( this->getCapabilities() & MO_MODULE_INPUT ) {
		std::cout << std::endl;
		std::cout << "Input :" << std::endl;
		for ( int i = 0; i < this->getInputCount(); i++ ) {
			std::cout << " " << i << ": name=" \
				<< this->getInputInfos(i)->getName() << ", type=" \
				<< this->getInputInfos(i)->getType() << ", desc=" \
				<< this->getInputInfos(i)->getDescription() << std::endl;
		}
	}

	if ( this->getCapabilities() & MO_MODULE_OUTPUT ) {
		std::cout << std::endl;
		std::cout << "Output :" << std::endl;
		for ( int i = 0; i < this->getOutputCount(); i++ ) {
			std::cout << " " << i << ": name=" \
				<< this->getOutputInfos(i)->getName() << ", type=" \
				<< this->getOutputInfos(i)->getType() << ", desc=" \
				<< this->getOutputInfos(i)->getDescription() << std::endl;
		}
	}

	std::cout << std::endl;
}

bool moModule::isPipeline() {
	return false;
}

bool moModule::haveError() {
	return this->is_error;
}

void moModule::setError(const std::string& msg) {
	this->error_msg = msg;
	this->is_error = true;
}

std::string moModule::getLastError() {
	std::ostringstream oss;
	this->is_error = false;
	oss << "<" << this->property("id").asString() << "> " << this->error_msg;
	return oss.str();
}

void moModule::poll() {
	if ( this->use_thread )
		return;
	if ( this->needUpdate() )
		this->update();
}

void moModule::notifyUpdate() {
	if ( this->use_thread ) {
		if ( sem_post(&this->sem_need_update) != 0 ) {
			LOGM(ERROR) << "unable to post semaphore";
			this->setError("unable to post semaphore");
		}
	} else {
		this->need_update = true;
	}
}

bool moModule::needUpdate(bool lock) {
	int err;

	// call from a thread
	if ( lock ) {
		do {
			err = sem_wait(&this->sem_need_update);
		} while ( err == -1 && errno == EINTR );
		if ( err != 0 )
			return false;
		return true;
	}

	// call not from a thread;
	bool b = this->need_update;
	this->need_update = false;
	return b;
}
