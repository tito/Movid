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

	this->id_counter = 1;
	this->new_blobs = new moDataGenericList();
	this->old_blobs = new moDataGenericList();
}

moAbstractBlobTrackerModule::~moAbstractBlobTrackerModule() {
	delete this->output;
}

void moAbstractBlobTrackerModule::pruneBlobs() {
	// Kill all the blobs that havn't been associated with a successor for too long
}

void moAbstractBlobTrackerModule::trackBlobs() {
	// For every blob in this->new_blobs, find the corresponding blob
	// from the last frame in this->old_blobs and assign it the same
	// ID. If no such blob exists, make it a new blob and give it a new ID.
	//
	// Uses this->calcWeight() to determine which blob belongs to which old blob.
}

double moAbstractBlobTrackerModule::calcWeight(moDataGenericContainer* old_blob,
											   moDataGenericContainer* new_blob) {
	// Computes the 'weight' or 'probability' that old_blob
	// (from frame n) is the same as new_blob (in frame n+1).
	// The function may just consider the euclidean distance
	// between the two blobs, but might take more sophisticated
	// decisions into account, e.g. if blob1 is a marker that
	// had the ID 123 and blob2 was identified by the blob finder
	// to have the ID 999, then the probability that blob1 is
	// blob2 is zero (or the weight is infinite).
	//
	// Returns a double. Lower value is better weight. 0.0 is best.
	return -1.;
}

void moAbstractBlobTrackerModule::update() {
	moDataGenericList::iterator it;
	moDataGenericList *blobs;
	std::string implements;

	// Clear new blob assignments from the last frame
	this->new_blobs->clear();
	// Get rid of old blobs that have went missing for too long
	// (e.g. if they weren't seen in the last $max_age frames.)
	this->pruneBlobs();

	// Copy the new blobs to our new_blobs list,
	// afterwards we'll assign ID's
	blobs = static_cast<moDataGenericList*>(this->input->getData());
	this->input->lock();
	for ( it = blobs->begin(); it != blobs->end(); it++ ) {
		moDataGenericContainer* blob = (*it)->clone();
		implements = blob->properties["implements"]->asString();
		// Indicate that the blob has been tracked, i.e. blob_id exists.
		implements += ",tracked";
		blob->properties["implements"]->set(implements);
		blob->properties["blob_id"] = new moProperty(0);
		this->new_blobs->push_back(blob);
	}
	this->input->unlock();

	// Try to find the blobs from the last $max_age frames in the new
	// frame and assign IDs accordingly.
	this->trackBlobs();

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

