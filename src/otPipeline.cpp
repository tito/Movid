//
// otPipeline.cpp
//
// Handle a group of object (pipeline)
//

#include <assert.h>
#include "otPipeline.h"
#include "otLog.h"

LOG_DECLARE("Pipeline");

MODULE_DECLARE_EX(Pipeline,, "native", "Handle object list");

otPipeline::otPipeline() : otModule(OT_MODULE_NONE, 0, 0) {
	MODULE_INIT();
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
	LOG(TRACE) << "add <" << module->property("id").asString() << "> to <" \
		<< this->property("id").asString() << ">";
	module->owner = this;
	this->modules.push_back(module);
}

void otPipeline::removeElement(otModule *module) {
	std::vector<otModule *>::iterator it;
	LOG(TRACE) << "remove <" << module->property("id").asString() << "> from <" \
		<< this->property("id").asString() << ">";
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( *it == module ) {
			this->modules.erase(it);
			break;
		}
	}
}

void otPipeline::setInput(otDataStream* stream, int n) {
	this->firstModule()->setInput(stream, n);
}

otDataStream* otPipeline::getInput(int n) {
	return this->lastModule()->getInput(n);
}

otDataStream* otPipeline::getOutput(int n) {
	return this->lastModule()->getOutput(n);
}

int otPipeline::getInputCount() {
	return this->firstModule()->getInputCount();
}

int otPipeline::getOutputCount() {
	return this->lastModule()->getOutputCount();
}

otDataStreamInfo *otPipeline::getInputInfos(int n) {
	return this->firstModule()->getInputInfos(n);
}

otDataStreamInfo *otPipeline::getOutputInfos(int n) {
	return this->lastModule()->getOutputInfos(n);
}

void otPipeline::start() {
	std::vector<otModule *>::iterator it;

	otModule::start();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->start();
	}
}

void otPipeline::stop() {
	std::vector<otModule *>::iterator it;

	otModule::stop();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->stop();
	}
}

void otPipeline::update() {
	std::vector<otModule *>::iterator it;

	LOGM(TRACE) << "update";

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->update();
	}
}

unsigned int otPipeline::size() {
	return this->modules.size();
}

otModule *otPipeline::getModule(unsigned int index) {
	assert( index >= 0 );
	assert( index < this->size() );

	return this->modules[index];
}

void otPipeline::setGroup(bool group) {
	this->is_group = group;
}

bool otPipeline::isGroup() {
	return this->is_group;
}

bool otPipeline::isPipeline() {
	return true;
}

