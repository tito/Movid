#include "otModule.h"
#include "otDataStream.h"

otModule::~otModule() {
	this->parent = NULL;
}

void otModule::update() {
}
