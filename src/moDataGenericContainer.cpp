#include "moDataGenericContainer.h"
#include "moProperty.h"

moDataGenericContainer::moDataGenericContainer() {
}

moDataGenericContainer::~moDataGenericContainer() {
	std::map<std::string, moProperty*>::iterator it;
	for ( it = this->properties.begin(); it != this->properties.end(); it++ )
		delete (it->second);
	this->properties.clear();
}

bool moDataGenericContainer::exist(const std::string &name) {
	std::map<std::string, moProperty*>::iterator it;
	it = this->properties.find(name);
	return it == this->properties.end() ? false : true;
}

