#include "otDataStream.h"
#include "otModule.h"

//I have to define these, even purely virtual doesnt work for destructor?  
//if not decalred and defined, child class compilation causes 'undefined' 
//error message when looking for base class de/con-structor?! 
otDataStream::otDataStream(){}
otDataStream::~otDataStream(){}


void otDataStream::update(){
}

void otDataStream::addObserver(otModule* module){
	this->observers.push_back(module);
}

void otDataStream::notifyObservers(){
	for(int i=0; i < this->observers.size(); i++){
		this->observers[i]->update();
	}
}




	
