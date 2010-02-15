#include <assert.h>

#include "otModule.h"
#include "otDataStream.h"

otModule::~otModule() {
	this->owner = NULL;
	this->is_started = false;
	this->input = NULL;
}

void otModule::update(otDataStream *source) {
}

void otModule::start() {
	this->is_started = true;
}

void otModule::stop() {
	this->is_started = false;
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

void otModule::setInput(otDataStream* input, int n) {
	assert( input != NULL );
	assert( n == 0);

	this->input = input;
	this->input->addObserver(this);
}

otDataStream* otModule::getOutput(int n) {
	assert( n == 0 );
	return this->input;
}

