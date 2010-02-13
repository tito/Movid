#include "otCamera.h"


otCamera::otCamera(int index){
	this->cam_capture = cvCaptureFromCAM(index);
	this->cam_frame   = NULL;
}
	
otCamera::~otCamera(){
	cvReleaseCapture(&this->cam_capture);	
}


IplImage* otCamera::getFrame(){
	this->update();
	return this->cam_frame;
}


void otCamera::update(){
	this->cam_frame = cvQueryFrame(this->cam_capture);
}