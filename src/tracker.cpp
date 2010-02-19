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

	otFactory::init();

	if ( argc > 1 )
		testname = argv[1];

	signal(SIGTERM, signal_term);
	signal(SIGINT, signal_term);

	otPipeline *pipeline = new otPipeline();

	if ( testname == "default" ) {

		// Camera input stream
		otModule* cam = otFactory::getInstance()->create("Video");

		otModule* gray = otFactory::getInstance()->create("GrayScale");
		gray->setInput(cam->getOutput());
		
		otModule* gauss = otFactory::getInstance()->create("Smooth");
		gauss->setInput(cam->getOutput());
		
		otModule* display = otFactory::getInstance()->create("ImageDisplay");
		display->setInput(gauss->getOutput());
		

		// order is important (specially the first and the last)
		pipeline->addElement(cam);
		pipeline->addElement(gray);
		pipeline->addElement(gauss);
		pipeline->addElement(display);

	} else if ( testname == "video" ) {

		otModule* video  = otFactory::getInstance()->create("Video");
		video->property("filename").set("media/blob2.avi");

		otModule* gray = otFactory::getInstance()->create("GrayScale");
		gray->setInput(video->getOutput());
		
		otModule* bg = otFactory::getInstance()->create("BackgroundSubtract");
		bg->setInput(gray->getOutput());
		
		otModule* smooth = otFactory::getInstance()->create("Smooth");
		smooth->setInput(bg->getOutput());
		
		otModule* thresh = otFactory::getInstance()->create("Threshold");
		thresh->setInput(smooth->getOutput());	

		
		otModule* blob = otFactory::getInstance()->create("BlobTracker");
		blob->setInput(thresh->getOutput());

		// otImageDisplayModule opens a window and displays an Image in it
		otModule* display = otFactory::getInstance()->create("ImageDisplay");
		display->property("name").set("GrayScale");
		display->setInput(smooth->getOutput());

		otModule* display2 = otFactory::getInstance()->create("ImageDisplay");
		display2->property("name").set("Threshold");
		display2->setInput(thresh->getOutput());
		
		otModule* display3 = otFactory::getInstance()->create("ImageDisplay");
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

