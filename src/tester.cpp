#include <iostream>
#include <signal.h>

#include "highgui.h"

#include "otParser.h"
#include "otPipeline.h"

LOG_DECLARE("Main");

static bool want_quit = false;

void usage(void) {
	std::cout << "Usage: tester \"... pipeline ...\"" << std::endl;
}

void signal_term(int signal) {
	want_quit = true;
	LOG(INFO) << "break, ask to stop";
}

int main(int argc, char **argv) {

	int key;
	otPipeline *pipeline;

	if ( argc != 2 )
	{
		usage();
		return 1;
	}

	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	pipeline = otParser::parseString(argv[1]);

	LOG(INFO) << "start the pipeline";

	pipeline->start();
	while ( !want_quit )
	{
		pipeline->update();

		// needed for imagedisplay...
		key = cvWaitKey(5);
	}

	LOG(INFO) << "stop the pipeline";
	pipeline->stop();

	return 0;
}
