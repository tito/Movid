#ifndef OT_CAMERA_DATA_STREAM_H
#define OT_CAMERA_DATA_STREAM_H

#include "highgui.h"
#include "otImageDataStream.h"

class otCameraDataStream :public otImageDataStream {

public:
	otCameraDataStream(int=0); 
	~otCameraDataStream();
	
	void update();

private:
	CvCapture* cam_capture;


};

#endif