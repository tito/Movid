#include <iostream>

#include "otModule.h"
#include "otFactory.h"

void describe(const char *name) {
	otModule *module;

	module = otFactory::getInstance()->create(name);
	if ( module == NULL ) {
		std::cerr << "Error: unable to found object named <" << name << ">" << std::endl;
		return;
	}

	module->describe();

	delete module;
}

int main(int argc, char **argv) {

	otFactory::init();

	if ( argc > 2 ) {
		std::cout << "Usage: " << argv[0] << " [objectname]" << std::endl;
		return 1;
	}

	if ( argc == 1 ) {
		std::vector<std::string>::iterator it;
		std::vector<std::string> list = otFactory::getInstance()->list();
		for ( it = list.begin(); it != list.end(); it++ )
			describe(it->c_str());
	} else {
		describe(argv[1]);
	}

	return 0;
}
