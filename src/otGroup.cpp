#include <assert.h>
#include "otGroup.h"

otGroup::otGroup() {
}

otGroup::~otGroup() {
}

void otGroup::addElement(otModule *module) {
	module->owner = this;
	this->modules.push_front(module);
}

void otGroup::removeElement(otModule *module) {
	// TODO removeElement
	assert(0);
}

void otGroup::setInput(otDataStream*, int n=0) {
	// TODO take children
	assert(0);
}

otDataStream* otGroup::getOutput(int n=0) {
	// TODO take children
	assert(0);
}

