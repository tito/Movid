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
#include "../moUtils.h"

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
	this->properties["min_age"] = new moProperty(3);
	// The maximum amount of degrees a blob may be rotated before we consider
	// the rotation having gone to the next quadrant.
	this->properties["max_rotation_speed"] = new moProperty(46);

	this->new_blobs = new moDataGenericList();
	this->old_blobs = new moDataGenericList();
	this->adolescent = new moDataGenericList();
}

moAbstractBlobTrackerModule::~moAbstractBlobTrackerModule() {
	delete this->output;
}

void moAbstractBlobTrackerModule::trackBlobs() {
	// For every blob in this->new_blobs, find the corresponding blob
	// from the last frame in this->old_blobs and assign it the same
	// ID. If no such blob exists, make it a new blob and give it a new ID.
}

void moAbstractBlobTrackerModule::trackAngles() {
	// For every blob in this->new_blobs, if that blob has a volatile angle,
	// we need to keep track of the rotation on the tracking level.
	// A blob has a volatile angle if we only get the blob's bounding box'
	// angle, which is always [0..90) and when you get to 90, it restarts at 0.
	// We reconstruct the real angle [0..360) from that by counting how often
	// the bounding box has "fallen over", i.e. how often there were severe jumps
	// with respect to the angle that were > max_rotation_speed, which we consider
	// as 'rotating from one quadrant to the next'. Camera FPS isn't unlimited...
	moDataGenericList::iterator it, oit;
	std::string implements;
	int angle, last_angle, diff, trackedangle, fallen_over, oid, id;
	int max_diff = this->property("max_rotation_speed").asDouble();
	bool in;

	for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++) {
		implements.clear();
		implements = (*it)->properties["implements"]->asString();
		if (!moUtils::inList("volatileangle", implements))
			continue;

		angle = moUtils::radToDeg((*it)->properties["angle"]->asDouble());

		id = (*it)->properties["blob_id"]->asInteger();
		in = false;
		for (oit = this->old_blobs->begin(); oit != this->old_blobs->end(); oit++) {
			oid = (*oit)->properties["blob_id"]->asInteger();
			if (oid == id) {
				in = true;
				break;
			}
		}
		//if (!(*it)->hasProperty("last_angle")) {
		if (!in) {
			// First time we want to track this blob's angle.
			(*it)->properties["last_angle"] = new moProperty(angle);
			(*it)->properties["fallen_over"] = new moProperty(0);
			continue;
		}
		fallen_over = (*oit)->properties["fallen_over"]->asInteger();
		last_angle = (*oit)->properties["last_angle"]->asInteger();

		diff = last_angle - angle;
		if (fabs((float)diff) > max_diff) {
			if (last_angle > angle)
				fallen_over++;
			else
				fallen_over--;
		}
		// %360+360 because otherwise it will not work for negative numbers,
		// different from e.g. python: >>> -23 % 360 == 337, but in cpp: -23
		trackedangle = ((angle + 90 * fallen_over) % 360 + 360) % 360;
		(*it)->properties["fallen_over"] = new moProperty(fallen_over);
		(*it)->properties["last_angle"] = new moProperty(angle);
		(*it)->properties["angle"]->set(moUtils::degToRad(trackedangle));
	}
}

void moAbstractBlobTrackerModule::update() {
	moDataGenericList::iterator it, oit;
	moDataGenericList *blobs;
	std::string implements;
	int max_age = this->property("max_age").asInteger();
	int min_age = this->property("min_age").asInteger();
	int age;

	// Clear new blob assignments from the last frame
	this->new_blobs->clear();
	this->adolescent->clear();

	for (it = this->old_blobs->begin(); it != this->old_blobs->end(); it++)
		(*it)->properties["age"]->set((*it)->properties["age"]->asInteger() + 1);

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

	// In case there are blobs with the flag 'volatileangle', the finder wants us
	// to track the angle of the blob.
	this->trackAngles();

	bool in;
	int oid, id;
	for (oit = this->old_blobs->begin(); oit != this->old_blobs->end(); oit++) {
		in = false;
		oid = (*oit)->properties["blob_id"]->asInteger();
		for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++) {
			id = (*it)->properties["blob_id"]->asInteger();
			if (oid == id) {
				in = true;
				break;
			}
		}
		// If a blob got lost, but not for too long, assume it's still there,
		// as long as it *was* there for some time already
		age = (*oit)->properties["age"]->asInteger();
		if (!in) {
			if ((age <= max_age) && (age >= min_age))
				this->new_blobs->push_back(*oit);
		}
		else
			(*it)->properties["age"]->set((min_age <= age) && (age <= max_age) ? min_age : age);
	}

	// Find all the blobs that are old enough
	for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++) {
		age = (*it)->properties["age"]->asInteger();
		if ((age <= max_age) && (age >= min_age))
			this->adolescent->push_back(*it);
	}

	// Send those blobs (possibly empty) down the pipeline
	this->output->push(this->adolescent);

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

