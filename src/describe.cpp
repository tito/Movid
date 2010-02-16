#include <iostream>

#include "otModule.h"
#include "otFactory.h"

int main(int argc, char **argv) {
	otModule *module;

	if ( argc != 2 ) {
		std::cout << "Usage: " << argv[0] << " objectname" << std::endl;
		return 1;
	}

	module = otFactory::create(argv[1]);
	if ( module == NULL ) {
		std::cerr << "Error: unable to found object named <" << argv[1] << ">" << std::endl;
		return 1;
	}

	module->describe();

	delete module;

	return 0;
}
