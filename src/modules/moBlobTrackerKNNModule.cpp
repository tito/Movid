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


#include "moBlobTrackerKNNModule.h"
#include "../moLog.h"

MODULE_DECLARE(BlobTrackerKNN, "native", "Track Blobs based on a KNN algorithm");

moBlobTrackerKNNModule::moBlobTrackerKNNModule() : moModule(MO_MODULE_INPUT | MO_MODULE_OUTPUT, 1, 2){

	MODULE_INIT();

	// initialize input/output
	this->input = NULL;
	this->output = NULL;

	// declare input/output
	this->input_infos[0] = new moDataStreamInfo("data", "moDataGenericList", "Data stream of type 'blob'");
	this->output_infos[0] = new moDataStreamInfo("data", "moDataGenericList", "Data stream of type 'blob'");
	this->output_infos[1] = new moDataStreamInfo("image", "IplImage", "Image showing the currently tracked blobs in different colors");
	// How many frames may a blob survive without finding a successor?
	this->properties["max_age"] = new moProperty(3);
	this->properties["max_dist"] = new moProperty(5.);
	this->id = 0;
}

moBlobTrackerKNNModule::~moBlobTrackerKNNModule() {
}

void moBlobTrackerKNNModule::pruneBlobs() {
	// Kill all the blobs that havn't been associated with a successor for too long
	int age;
	int maxAge = this->property("max_age").asInteger();
	std::vector<int> erase;
	moDataGenericContainer *cur_blob;
	std::vector<moDataGenericContainer*> alive_blobs;
	for (unsigned int i = 0; i < this->old_blob_pool.size(); i++) {
		cur_blob = old_blob_pool[i];	
		age = cur_blob->properties["age"]->asInteger();
		if (age + 1 > maxAge)
			return;
		else {
			cur_blob->properties["age"]->set(age + 1);
			alive_blobs.push_back(cur_blob);
		}
	}
	this->old_blob_pool = alive_blobs;
}

void moBlobTrackerKNNModule::track(moDataGenericList *new_blobs) {
	// XXX:
	// Really simple & stupid algo right now. Just a stub for testing.
	// To be replaced by a proper tracking algo later.
	moDataGenericList::iterator it;
	int id;
	double min, dist, max_dist = this->property("max_dist").asDouble();
	moDataGenericContainer *old, *smallest;
	int new_x, new_y, old_x, old_y;
	for (it=new_blobs->begin(); it != new_blobs->end(); it++) {
		// Initialize
		(*it)->properties["id"]->set(-1);
	}
	for (unsigned int i = 0; i < this->old_blob_pool.size(); i++) {
		// Try to reuse blobs
		smallest = NULL;
		min = 0;
		old = this->old_blob_pool[i];
		for (it=new_blobs->begin(); it != new_blobs->end(); it++) {
			old_x = old->properties["x"]->asInteger();
			old_y = old->properties["y"]->asInteger();
			new_x = (*it)->properties["x"]->asInteger();
			new_y = (*it)->properties["y"]->asInteger();
			dist = pow(old_x - new_x, 2) + pow(old_y - new_y, 2);
			if (dist <= min) {
				smallest = *it;
				min = dist;
			}
		}
		if ((smallest != NULL) && (min <= max_dist)) {
			id = old->properties["id"]->asInteger();
			smallest->properties["id"]->set(id);
		}
	}
	for (it=new_blobs->begin(); it != new_blobs->end(); it++) {
		// New blob, new ID
		id = (*it)->properties["id"]->asInteger();
		if (id == -1)
			(*it)->properties["id"]->set(this->id++);
	}
}

void moBlobTrackerKNNModule::update() {
	// XXX does input locking still apply?
	assert(this->input != NULL);

	moDataGenericList *blobs = (moDataGenericList*) this->input->getData();

//	this->input->lock();
//
//	this->pruneBlobs();
//	moDataGenericList *new_blobs = static_cast<moDataGenericList*>(this->input->getData());
//	this->track(new_blobs);
//	this->output->push(new_blobs);
//
//
//
//
//	moDataGenericList::iterator it;
//	for (it=new_blobs->begin(); it != new_blobs->end(); it++) {
//		std::cout << (*it)->properties["id"]->asInteger() << std::endl;
//	}
//
//
//
//	this->input->unlock();	
}

void moBlobTrackerKNNModule::notifyData(moDataStream *input) {
	assert( input != NULL );
	assert( input == this->input );
	this->notifyUpdate();
}

void moBlobTrackerKNNModule::start() {
	moModule::start();
}

void moBlobTrackerKNNModule::stop() {
	moModule::stop();
}

void moBlobTrackerKNNModule::setInput(moDataStream *stream, int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return;
	}
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( stream != NULL ) {
		if ( stream->getFormat() != "GenericBlob" ) {
			this->setError("Input 0 accept only blobs");
			this->input = NULL;
			return;
		}
	}
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moBlobTrackerKNNModule::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moBlobTrackerKNNModule::getOutput(int n) {
	return this->output;
}

