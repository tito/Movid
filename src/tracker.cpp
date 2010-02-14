#include <cstdio>

#include "cv.h"
#include "highgui.h"
#include "otCameraDataStream.h"
#include "otImageDisplayModule.h"


int main(int argc, char **argv)
{
	int key;

	otCameraDataStream* cam  = new otCameraDataStream(0);
	otImageDisplayModule* display = new otImageDisplayModule("OpenTracker");
	otImageDisplayModule* display2 = new otImageDisplayModule("Window 2");
	display->setInput(cam);
	display2->setInput(display->getOutput());
	
	while ( true )
	{
		cam->update();

		key = cvWaitKey(5);
		if ( key==0x1b )
			break;
	}

	cvDestroyWindow("OpenTracker");

	return 0;
}
