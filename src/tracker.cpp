#include <iostream>
#include <signal.h>

#include "highgui.h"

#include "otModule.h"
#include "otDataStream.h"
#include "otFactory.h"
#include "otProperty.h"
#include "otPipeline.h"

static bool want_quit = false;

void signal_term(int signal) {
	want_quit = true;
}

int main(int argc, char **argv) {
	int key = 0x0;

	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	otPipeline *pipeline = new otPipeline();

	// Camera input stream
	otModule* cam  = otFactory::create("Camera");

	otModule* gauss = otFactory::create("Smooth");
	gauss->setInput(cam->getOutput(0));
	gauss->property("width").set(11);
	gauss->property("height").set(11);
	gauss->property("filter").set("OT_MEDIAN");

	// otImageDisplayModule opens a window and displays an Image in it
	otModule* display = otFactory::create("ImageDisplay");
	display->setInput(gauss->getOutput(0));

	// order is important (specially the first and the last)
	pipeline->addElement(cam);
	pipeline->addElement(gauss);
	pipeline->addElement(display);

	pipeline->start();

	// keep updating teh camera until ESC key is pressed
	while ( key != 0x1b && !want_quit ) {
		pipeline->update();
		key = cvWaitKey(5);
	}

	pipeline->stop();

	return 0;
}

