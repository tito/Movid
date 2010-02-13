#include <cstdio>

#include "otCamera.h"
#include "highgui.h"

int main(int argc, char **argv)
{
	int key;

	otCamera* cam	= new otCamera(0);
	
	cvNamedWindow("OpenTracker", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("OpenTracker", 50, 50);

	while ( true )
	{
		cvShowImage("Capture", cam->getFrame());

		key = cvWaitKey(10);
		if ( key==0x1b )
			break;
	}

	cvDestroyWindow("Capture");

	return 0;
}
