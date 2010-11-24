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


#include "moAbstractBlobTrackerModule.h"
#include "../moLog.h"

MODULE_DECLARE(AbstractBlobTracker, "native", "Abstract blob tracking module that makes adding new trackers easy.");

int moAbstractBlobTrackerModule::id_counter = 0;

moAbstractBlobTrackerModule::moAbstractBlobTrackerModule() : moModule(MO_MODULE_INPUT | MO_MODULE_OUTPUT) {

	MODULE_INIT();

	// initialize input/output
	this->input = NULL;
	this->output = new moDataStream("trackedblob");

	// declare input/output
	this->declareInput(0, &this->input, new moDataStreamInfo(
				"data", "blob", "Data stream of type 'blob'"));
	this->declareOutput(0, &this->output, new moDataStreamInfo(
				"data", "trackedblob", "Data stream of type 'trackedblob'"));

	this->properties["max_age"] = new moProperty(10);

	this->new_blobs = new moDataGenericList();
	this->old_blobs = new moDataGenericList();
}

moAbstractBlobTrackerModule::~moAbstractBlobTrackerModule() {
	delete this->output;
}

void moAbstractBlobTrackerModule::trackBlobs() {
	// For every blob in this->new_blobs, find the corresponding blob
	// from the last frame in this->old_blobs and assign it the same
	// ID. If no such blob exists, make it a new blob and give it a new ID.
}

void moAbstractBlobTrackerModule::update() {
	moDataGenericList::iterator it, oit;
	moDataGenericList *blobs;
	std::string implements;
	int max_age = this->property("max_age").asInteger();

	// Clear new blob assignments from the last frame
	this->new_blobs->clear();

	for (it = this->old_blobs->begin(); it != this->old_blobs->end(); it++) {
		(*it)->properties["age"]->set((*it)->properties["age"]->asInteger() + 1);
	}

	// Copy the new blobs to our new_blobs list,
	// afterwards we'll assign ID's
	blobs = static_cast<moDataGenericList*>(this->input->getData());
	this->input->lock();
	for (it = blobs->begin(); it != blobs->end(); it++) {
		moDataGenericContainer* blob = (*it)->clone();
		implements = blob->properties["implements"]->asString();
		// Indicate that the blob has been tracked, i.e. blob_id exists.
		implements += ",tracked";
		blob->properties["implements"]->set(implements);
		blob->properties["blob_id"] = new moProperty(0);
		blob->properties["age"] = new moProperty(0);
		this->new_blobs->push_back(blob);
	}
	this->input->unlock();

	// Try to find the blobs from the last $max_age frames in the new
	// frame and assign IDs accordingly.
	this->trackBlobs();

	for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++)
		if ((*it)->properties["blob_id"] > 0)
			// The blob was just found, so it is alive. Reset age to 0.
			(*it)->properties["age"]->set(0);

	// Remember blobs even if they get lost for a while
	bool in;
	int oid, id;
	for (oit = this->old_blobs->begin(); oit != this->old_blobs->end(); oit++) {
		in = false;
		oid = (*oit)->properties["blob_id"]->asInteger();
		for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++) {
			id = (*it)->properties["blob_id"]->asInteger();
			if (oid == id)
				in = true;
		}
		if (!in && ((*oit)->properties["age"]->asInteger() <= max_age)) {
			this->new_blobs->push_back(*oit);
		} else {
		        delete (*oit);
		}
	}

	// Send the new blobs (possibly empty) down the pipeline
	this->output->push(this->new_blobs);

	// Make the new list the old list for the next frame
	moDataGenericList* tmp = this->old_blobs;
	this->old_blobs = this->new_blobs;
	this->new_blobs = tmp;
}

void moAbstractBlobTrackerModule::notifyData(moDataStream *input) {
	assert( input != NULL );
	assert( input == this->input );
	this->notifyUpdate();
}

