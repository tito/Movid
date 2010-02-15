#include <iostream>
#include "otParser.h"

void usage(void) {
	std::cout << "Usage: tester \"... pipeline ...\"" << std::endl;
	exit(1);
}

int main(int argc, char **argv) {

	if ( argc != 2 )
		usage();

	otParser::parseString(argv[1]);

	return 0;
}
