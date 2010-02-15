#include <iostream>

#include "highgui.h"

#include "otModule.h"
#include "otDataStream.h"
#include "otFactory.h"
#include "otProperty.h"

int main(int argc, char **argv){
	int key = 0x0;


	// ------ Test properties
	otProperty p1 = otProperty("integer", 5),
			   p2 = otProperty("string", "plop"),
			   p3 = otProperty("boolean", false),
			   p4 = otProperty("double", 98.654);

	std::cout << p1 << "," << p2 << "," << p3 << "," << p4 << std::endl;
	// ------ End test

	// Camera input stream
	otModule* cam  = otFactory::create("Camera");

	// FIXME, should be done by a pipeline
	cam->start();

	otModule* gauss = otFactory::create("GaussianBlur");
	gauss->setInput(cam->getOutput(0));

	// otImageDisplayModule opens a window and displays an Image in it
	otModule* display = otFactory::create("ImageDisplay");
	display->setInput(gauss->getOutput(0));

	// keep updating teh camera until ESC key is pressed
	while ( key != 0x1b ) {
		cam->update();
		key = cvWaitKey(5);
	}

	return 0;
}

