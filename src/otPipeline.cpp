//
// otPipeline.cpp
//
// Handle a group of object (pipeline)
//

#include <assert.h>
#include "otPipeline.h"
#include "otLog.h"

LOG_DECLARE("Group");

MODULE_DECLARE_EX(Pipeline,, "native", "Handle object list");

otPipeline::otPipeline() : otModule(OT_MODULE_NONE, 0, 0) {
}

otPipeline::~otPipeline() {
}

otModule *otPipeline::firstModule() {
	assert( this->modules.size() > 0 );
	return this->modules[0];
}

otModule *otPipeline::lastModule() {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1];
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
	this->firstModule()->setInput(stream, n);
}

otDataStream* otPipeline::getOutput(int n=0) {
	return this->lastModule()->getOutput(n);
}

int otPipeline::getInputCount() {
	return this->firstModule()->getInputCount();
}

int otPipeline::getOutputCount() {
	return this->lastModule()->getOutputCount();
}

std::string otPipeline::getInputName(int n) {
	return this->firstModule()->getInputName(n);
}

std::string otPipeline::getOutputName(int n) {
	return this->lastModule()->getOutputName(n);
}

std::string otPipeline::getInputType(int n) {
	return this->firstModule()->getInputType(n);
}

std::string otPipeline::getOutputType(int n) {
	return this->lastModule()->getOutputType(n);
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
