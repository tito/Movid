#include "otCameraDataStream.h"

otCameraDataStream::otCameraDataStream(int index){
	this->cam_capture = cvCaptureFromCAM(index);
	this->image   = NULL;
}
	
otCameraDataStream::~otCameraDataStream(){
	cvReleaseCapture(&this->cam_capture);	
}


void otCameraDataStream::update(){
	this->setImageData( cvQueryFrame(this->cam_capture) );
}