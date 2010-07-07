/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#include "moGreedyBlobTrackerModule.h"
#include "../moLog.h"

MODULE_DECLARE(GreedyBlobTracker, "native", "Track Blobs based on a simple greedy algorithm");

moGreedyBlobTrackerModule::moGreedyBlobTrackerModule() : moModule(MO_MODULE_INPUT | MO_MODULE_OUTPUT, 1, 1){

	MODULE_INIT();

	// initialize input/output
	this->input = NULL;
	this->output = new moDataStream("blob");

	// declare input/output
	this->input_infos[0] = new moDataStreamInfo("data", "moDataGenericList", "Data stream of type 'blob'");
	this->output_infos[0] = new moDataStreamInfo("data", "moDataGenericList", "Data stream of type 'blob'");
	//this->output_infos[1] = new moDataStreamInfo("image", "IplImage", "Image showing the currently tracked blobs in different colors");

    // How many frames may a blob survive without finding a successor?
	this->properties["max_age"] = new moProperty(48);
	this->properties["max_dist"] = new moProperty(0.2);

    this->id_counter = 1;
    this->new_blobs = new moDataGenericList();
    this->old_blobs = new moDataGenericList();

}

moGreedyBlobTrackerModule::~moGreedyBlobTrackerModule() {
    delete this->output;
}

void moGreedyBlobTrackerModule::pruneBlobs() {
	// Kill all the blobs that havn't been associated with a successor for too long
}

void moGreedyBlobTrackerModule::trackBlobs() {
    moDataGenericList::iterator it;
	for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++){
        //for each of blobs in teh new frame, find teh closest matching one from before
        moDataGenericContainer* closest_blob = NULL;
        double min_dist = pow(this->properties["max_dist"]->asDouble(), 2);
        
        moDataGenericList::iterator it_old;
	    for (it_old = this->old_blobs->begin(); it_old != this->old_blobs->end(); it_old++){
            LOG(MO_DEBUG, "old blob:" << (*it_old)->properties["blob_id"]->asString() <<"  x:" << (*it)->properties["x"]->asDouble() );
            if ((*it_old)->properties["blob_id"]->asInteger() < 0)  //already assigned
                continue;

            double old_x = (*it_old)->properties["x"]->asDouble();
			double old_y = (*it_old)->properties["y"]->asDouble();
			double new_x = (*it)->properties["x"]->asDouble();
			double new_y = (*it)->properties["y"]->asDouble();
			// XXX Make sure that our INPUT is in 0.0 - 1.0. I checked and it seemed to not always be...
			double dist = pow(old_x - new_x, 2) + pow(old_y - new_y, 2);
            
            if (dist < min_dist) {
                closest_blob = (*it_old);
                min_dist = dist;
            }
        }
        
        //found the closest one out of teh ones that are left, assign id, and invalidate old blob
        if (closest_blob){
            int old_id = closest_blob->properties["blob_id"]->asInteger();
            (*it)->properties["blob_id"]->set( old_id );
            closest_blob->properties["blob_id"]->set( -1 * old_id );  //we mark matched blob by negative id's
        }
        //this must be a new blob, so assign new ID
        else{
            (*it)->properties["blob_id"]->set(++this->id_counter);
        }
    }
}

void moGreedyBlobTrackerModule::update() {
    LOG(MO_DEBUG, "update called");
    
    this->new_blobs->clear();
   
    //copy teh new blobs to our new_blobs list, afterwards well assign id's 
    moDataGenericList::iterator it;
	moDataGenericList *blobs = (moDataGenericList*) this->input->getData();

    this->input->lock();
	for ( it = blobs->begin(); it != blobs->end(); it++ ){
        moDataGenericContainer* blob = (*it)->clone();
        blob->properties["blob_id"] = new moProperty(0);
        this->new_blobs->push_back(blob);   
    }
    this->input->unlock();

   //trck the blobs based on prior frames
   this->trackBlobs();

   //revive old bobs that just went missing if they are within certain age 
   //(in case they only dropped out for one or two frames)
   //this->pruneBlobs();

   this->output->push(this->new_blobs);
   
   //make teh new list teh old list
   moDataGenericList* tmp = this->old_blobs;
   this->old_blobs = this->new_blobs;
   this->new_blobs = tmp;
}

void moGreedyBlobTrackerModule::notifyData(moDataStream *input) {
	assert( input != NULL );
	assert( input == this->input );
	this->notifyUpdate();
}


void moGreedyBlobTrackerModule::setInput(moDataStream *stream, int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return;
	}
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( stream != NULL ) {
		if ( stream->getFormat() != "blob" ) {
			this->setError("Input 0 only accepts blobs but got " + stream->getFormat());
			this->input = NULL;
			return;
		}
	}
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moGreedyBlobTrackerModule::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moGreedyBlobTrackerModule::getOutput(int n) {
	return this->output;
}

