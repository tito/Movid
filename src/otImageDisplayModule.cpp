#include "otImageDisplayModule.h"

#include "highgui.h"
#include "otImageDataStream.h"


otImageDisplayModule::otImageDisplayModule(char* name){
	this->window_name = new std::string(name);
	cvNamedWindow(this->window_name->c_str(), CV_WINDOW_AUTOSIZE);
	cvMoveWindow(this->window_name->c_str(), 50, 50);
}

otImageDisplayModule::~otImageDisplayModule(){
	cvDestroyWindow(this->window_name->c_str());
	delete this->window_name;
}


void otImageDisplayModule::update(){
	cvShowImage(this->window_name->c_str(), this->input->getImageData());
}


void otImageDisplayModule::setInput( otDataStream* in, int n){
	this->input = (otImageDataStream*)in;
	this->input->addObserver(this);
}


otDataStream* otImageDisplayModule::getOutput( int n){
	return this->input; //pass through
}
	

