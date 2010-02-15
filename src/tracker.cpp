#include <iostream>

#include "highgui.h"
#include "otCamera.h"
#include "otDataStream.h"
#include "otImageDisplayModule.h"
#include "otGaussianBlurModule.h"
#include "otModule.h"
#include "otFactory.h"

int main(int argc, char **argv){
	int key;

	// Camera input stream
	otModule* cam  = otFactory::create("otCamera");

	// FIXME, should be done by a pipeline
	cam->start();

	otGaussianBlurModule* gauss = new otGaussianBlurModule();
	gauss->setInput(cam->getOutput(0));

	
	// otImageDisplayModule opens a window and displays an Image in it
	otImageDisplayModule* display = new otImageDisplayModule();
	display->setInput(gauss->getOutput(0));
	

	// keep updating teh camera until ESC key is pressed
	while ( key != 0x1b ) {
		cam->update();
		key = cvWaitKey(5);
	}

	return 0;
}

