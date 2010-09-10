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

moGreedyBlobTrackerModule::moGreedyBlobTrackerModule() : moAbstractBlobTrackerModule() {
	MODULE_INIT();

	this->properties["max_distance"] = new moProperty(0.1);
}

void moGreedyBlobTrackerModule::trackBlobs() {
	moDataGenericList::iterator it;
	double distance, min_distance, old_x, old_y, new_x, new_y;
	for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++){
		// For each blob from the new frame, find the closest one from the old frame(s)
		moDataGenericContainer* closest_blob = NULL;
		min_distance = this->properties["max_distance"]->asDouble();

		new_x = (*it)->properties["x"]->asDouble();
		new_y = (*it)->properties["y"]->asDouble();
		moDataGenericList::iterator it_old;
		for (it_old = this->old_blobs->begin(); it_old != this->old_blobs->end(); it_old++){
			if ((*it_old)->properties["blob_id"]->asInteger() < 0)
				// Blob was already assigned, i.e. the ID was already reused.
				continue;

			// FIXME Make sure that our INPUT is in 0.0 - 1.0. I checked and it seemed to not always be...
			old_x = (*it_old)->properties["x"]->asDouble();
			old_y = (*it_old)->properties["y"]->asDouble();
			distance = sqrt(pow(old_x - new_x, 2) + pow(old_y - new_y, 2));

			if (distance < min_distance) {
				closest_blob = (*it_old);
				min_distance = distance;
			}
		}

		//found the closest one out of teh ones that are left, assign id, and invalidate old blob
		if (closest_blob){
			int old_id = closest_blob->properties["blob_id"]->asInteger();
			(*it)->properties["blob_id"]->set( old_id );
			closest_blob->properties["blob_id"]->set( -1 * old_id );  //we mark matched blob by negative id's
		}
		//this must be a new blob, so assign new ID
		else
			// Atomically increment counter in case we got more than one tracker
			(*it)->properties["blob_id"]->set(pt::pincrement(&(moAbstractBlobTrackerModule::id_counter)));
	}
}

