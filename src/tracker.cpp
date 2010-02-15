#include "highgui.h"
#include "otCamera.h"
#include "otImageDisplayModule.h"

int main(int argc, char **argv){
	int key;

	// Camera input stream
	otCamera* cam  = new otCamera();

	// otImageDisplayModule opens a window and displays an Image in it
	otImageDisplayModule* display = new otImageDisplayModule("OpenTracker");
	display->setInput(cam->getOutput(0));

	// simple pass through to test input/output pipes
	otImageDisplayModule* display2 = new otImageDisplayModule("Window 2");
	display2->setInput(display->getOutput());

	// keep updating teh camera until ESC key is pressed
	while ( key != 0x1b ) {
		cam->update();
		key = cvWaitKey(5);
	}

	return 0;
}

