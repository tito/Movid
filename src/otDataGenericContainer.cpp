#include "otDataGenericContainer.h"
#include "otProperty.h"

otDataGenericContainer::otDataGenericContainer() {
}

otDataGenericContainer::~otDataGenericContainer() {
	std::map<std::string, otProperty*>::iterator it;
	for ( it = this->properties.begin(); it != this->properties.end(); it++ )
		delete (it->second);
	this->properties.clear();
}

bool otDataGenericContainer::exist(const std::string &name) {
	std::map<std::string, otProperty*>::iterator it;
	it = this->properties.find(name);
	return it == this->properties.end() ? false : true;
}

