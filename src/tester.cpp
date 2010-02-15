#include <iostream>
#include "otParser.h"

void usage(void) {
	std::cout << "Usage: tester \"... pipeline ...\"" << std::endl;
}

int main(int argc, char **argv) {

	if ( argc != 2 )
	{
		usage();
		return 1;
	}

	otParser::parseString(argv[1]);

	return 0;
}
