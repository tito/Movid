#include "highgui.h"
#include "otCameraDataStream.h"
#include "otImageDisplayModule.h"

int main(int argc, char **argv){
	int key;

	//Camera input stream, when its updated, it automatically updates anything its connected to
	otCameraDataStream* cam  = new otCameraDataStream(0);
	
	//otImageDisplayModule opens a window and displays an Image in it
	otImageDisplayModule* display = new otImageDisplayModule("OpenTracker");
	display->setInput(cam);
	
	//simple pass through to test input/output pipes
	otImageDisplayModule* display2 = new otImageDisplayModule("Window 2");
	display2->setInput(display->getOutput()); 
	
	//keep updating teh camera until ESC key is pressed
	while ( key != 0x1b ){
		cam->update();
		key = cvWaitKey(5);
	}

	return 0;
}
