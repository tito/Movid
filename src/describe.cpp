#include <iostream>

#include "moModule.h"
#include "moFactory.h"

void describe(const char *name) {
	moModule *module;

	module = moFactory::getInstance()->create(name);
	if ( module == NULL ) {
		std::cerr << "Error: unable to found object named <" << name << ">" << std::endl;
		return;
	}

	module->describe();

	delete module;
}

int main(int argc, char **argv) {

	moFactory::init();

	if ( argc > 2 ) {
		std::cout << "Usage: " << argv[0] << " [objectname]" << std::endl;
		return 1;
	}

	if ( argc == 1 ) {
		std::vector<std::string>::iterator it;
		std::vector<std::string> list = moFactory::getInstance()->list();
		for ( it = list.begin(); it != list.end(); it++ )
			describe(it->c_str());
	} else {
		describe(argv[1]);
	}

	return 0;
}
