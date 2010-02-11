#include <cstdio>
#include "cv.h"
#include "highgui.h"

int main(int argc, char **argv)
{
	int key;

	CvCapture* capture	= cvCaptureFromCAM(0);
	IplImage* frame		= NULL;

	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	cvMoveWindow("Capture", 50, 50);

	while ( true )
	{
		frame = cvQueryFrame(capture);
		if ( frame == NULL )
			break;

		cvShowImage("Capture", frame);

		key = cvWaitKey(10);
		if ( key==0x1b )
			break;
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("Capture");

	return 0;
}
