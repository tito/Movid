#include <iostream>
#include <signal.h>

#include "highgui.h"

#include "moModule.h"
#include "moDataStream.h"
#include "moFactory.h"
#include "moProperty.h"
#include "moPipeline.h"

static bool want_quit = false;

void signal_term(int signal) {
	want_quit = true;
}

int main(int argc, char **argv) {
	int key = 0x0;
	std::string testname = "default";

	moFactory::init();

	if ( argc > 1 )
		testname = argv[1];

	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	moPipeline *pipeline = new moPipeline();

	if ( testname == "default" ) {

		// Camera input stream
		moModule* cam = moFactory::getInstance()->create("Video");

		moModule* gray = moFactory::getInstance()->create("GrayScale");
		gray->setInput(cam->getOutput());
		
		moModule* gauss = moFactory::getInstance()->create("Smooth");
		gauss->setInput(cam->getOutput());
		
		moModule* display = moFactory::getInstance()->create("ImageDisplay");
		display->setInput(gauss->getOutput());
		

		// order is important (specially the first and the last)
		pipeline->addElement(cam);
		pipeline->addElement(gray);
		pipeline->addElement(gauss);
		pipeline->addElement(display);

	} else if ( testname == "video" ) {

		moModule* video  = moFactory::getInstance()->create("Video");
		video->property("filename").set("media/blob2.avi");

		moModule* gray = moFactory::getInstance()->create("GrayScale");
		gray->setInput(video->getOutput());
		
		moModule* bg = moFactory::getInstance()->create("BackgroundSubtract");
		bg->setInput(gray->getOutput());
		
		moModule* smooth = moFactory::getInstance()->create("Smooth");
		smooth->setInput(bg->getOutput());
		
		moModule* thresh = moFactory::getInstance()->create("Threshold");
		thresh->setInput(smooth->getOutput());	

		
		moModule* blob = moFactory::getInstance()->create("BlobTracker");
		blob->setInput(thresh->getOutput());

		// moImageDisplayModule opens a window and displays an Image in it
		moModule* display = moFactory::getInstance()->create("ImageDisplay");
		display->property("name").set("GrayScale");
		display->setInput(smooth->getOutput());

		moModule* display2 = moFactory::getInstance()->create("ImageDisplay");
		display2->property("name").set("Threshold");
		display2->setInput(thresh->getOutput());
		
		moModule* display3 = moFactory::getInstance()->create("ImageDisplay");
		display3->property("name").set("Blobs");
		display3->setInput(blob->getOutput());

		// order is important (specially the first and the last)
		pipeline->addElement(video);
		pipeline->addElement(gray);
		pipeline->addElement(bg);
		pipeline->addElement(smooth);
		pipeline->addElement(thresh);
		pipeline->addElement(blob);
		pipeline->addElement(display);
		pipeline->addElement(display2);
		pipeline->addElement(display3);

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

