#include "otImageDataStream.h"

void otImageDataStream::setImageData(IplImage* data){
	this->image = data;
	this->notifyObservers();
}

IplImage* otImageDataStream::getImageData(){
	return this->image;
}
	


