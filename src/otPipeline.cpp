//
// otPipeline.cpp
//
// Handle a group of object (pipeline)
//

#include <assert.h>
#include "otPipeline.h"
#include "otLog.h"

LOG_DECLARE("Group");

otPipeline::otPipeline() : otModule(OT_MODULE_NONE, 0, 0) {
}

otPipeline::~otPipeline() {
}

void otPipeline::addElement(otModule *module) {
	assert( module != NULL );
	module->owner = this;
	this->modules.push_back(module);
}

void otPipeline::removeElement(otModule *module) {
	std::vector<otModule *>::iterator it;
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( *it == module ) {
			this->modules.erase(it);
			break;
		}
	}
}

void otPipeline::setInput(otDataStream* stream, int n=0) {
	assert( this->modules.size() > 0 );
	this->modules[0]->setInput(stream, n);
}

otDataStream* otPipeline::getOutput(int n=0) {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutput(n);
}

int otPipeline::getInputCount() {
	assert( this->modules.size() > 0 );
	return this->modules[0]->getInputCount();
}

int otPipeline::getOutputCount() {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutputCount();
}

std::string otPipeline::getInputName(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[0]->getInputName(n);
}

std::string otPipeline::getOutputName(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutputName(n);
}

std::string otPipeline::getInputType(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[0]->getInputType(n);
}

std::string otPipeline::getOutputType(int n) {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1]->getOutputType(n);
}

void otPipeline::start() {
	std::vector<otModule *>::iterator it;

	LOG(INFO) << "Start the pipeline";

	otModule::start();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->start();
	}
}

void otPipeline::stop() {
	std::vector<otModule *>::iterator it;

	LOG(INFO) << "Stop the pipeline";

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->stop();
	}

	otModule::stop();
}

void otPipeline::update() {
	std::vector<otModule *>::iterator it;

	LOG(DEBUG) << "Update the pipeline";

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->update();
	}
}
