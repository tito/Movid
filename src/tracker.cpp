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
	std::string testname = "default";

	if ( argc > 1 )
		testname = argv[1];

	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	otPipeline *pipeline = new otPipeline();

	if ( testname == "default" ) {

		// Camera input stream
		otModule* cam = otFactory::create("Camera");

		otModule* gauss = otFactory::create("Smooth");
		gauss->setInput(cam->getOutput());
		gauss->property("width").set(11);
		gauss->property("height").set(11);
		gauss->property("filter").set("blur");

		// otImageDisplayModule opens a window and displays an Image in it
		otModule* display = otFactory::create("ImageDisplay");
		display->setInput(gauss->getOutput());

		// order is important (specially the first and the last)
		pipeline->addElement(cam);
		pipeline->addElement(gauss);
		pipeline->addElement(display);

	} else if ( testname == "video" ) {

		otModule* video  = otFactory::create("Video");
		video->property("filename").set("media/blob.avi");

		otModule* invert = otFactory::create("Invert");
		invert->setInput(video->getOutput());

		// otImageDisplayModule opens a window and displays an Image in it
		otModule* display = otFactory::create("ImageDisplay");
		display->property("name").set("OpenTracker result");
		display->setInput(invert->getOutput());

		otModule* displaysrc = otFactory::create("ImageDisplay");
		displaysrc->property("name").set("OpenTracker source");
		displaysrc->setInput(video->getOutput());

		// order is important (specially the first and the last)
		pipeline->addElement(video);
		pipeline->addElement(invert);
		pipeline->addElement(display);
		pipeline->addElement(displaysrc);

	}

	pipeline->start();

	// keep updating teh camera until ESC key is pressed
	while ( key != 0x1b && !want_quit ) {
		pipeline->update();
		key = cvWaitKey(5);
	}

	pipeline->stop();

	return 0;
}

