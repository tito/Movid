//
// otGroup.cpp
//
// Handle a group of object (pipeline)
//

#include <assert.h>
#include "otGroup.h"
#include "otLog.h"

LOG_DECLARE("Group");

otGroup::otGroup() : otModule(OT_MODULE_NONE, 0, 0) {
}

otGroup::~otGroup() {
}

void otGroup::addElement(otModule *module) {
	assert( module != NULL );
	module->owner = this;
	this->modules.push_back(module);
}

void otGroup::removeElement(otModule *module) {
	std::vector<otModule *>::iterator it;
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( *it == module ) {
			this->modules.erase(it);
			break;
		}
	}
}

void otGroup::setInput(otDataStream* stream, int n=0) {
	assert( this->modules.size() > 0 );
	this->modules[0]->setInput(stream, n);
}

otDataStream* otGroup::getOutput(int n=0) {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutput(n);
}

int otGroup::getInputCount() {
	assert( this->modules.size() > 0 );
	return this->modules[0]->getInputCount();
}

int otGroup::getOutputCount() {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutputCount();
}

std::string otGroup::getInputName(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[0]->getInputName(n);
}

std::string otGroup::getOutputName(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutputName(n);
}

std::string otGroup::getInputType(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[0]->getInputType(n);
}

std::string otGroup::getOutputType(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutputType(n);
}

void otGroup::start() {
	std::vector<otModule *>::iterator it;

	LOG(INFO) << "Start the pipeline";

	otModule::start();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->start();
	}
}

void otGroup::stop() {
	std::vector<otModule *>::iterator it;

	LOG(INFO) << "Stop the pipeline";

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->stop();
	}

	otModule::stop();
}

void otGroup::update() {
	std::vector<otModule *>::iterator it;

	LOG(DEBUG) << "Update the pipeline";

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->update();
	}
}
