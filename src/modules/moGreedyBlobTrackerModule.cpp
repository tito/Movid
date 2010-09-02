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

	this->properties["max_weight"] = new moProperty(0.1);
}

double moGreedyBlobTrackerModule::calcWeight(moDataGenericContainer* old_blob,
											 moDataGenericContainer* new_blob) {
	// Just take the euclidean distance between the two blobs
	double old_x, old_y, new_x, new_y;
	old_x = old_blob->properties["x"]->asDouble();
	old_y = old_blob->properties["y"]->asDouble();
	new_x = new_blob->properties["x"]->asDouble();
	new_y = new_blob->properties["y"]->asDouble();
	return sqrt(pow(old_x - new_x, 2) + pow(old_y - new_y, 2));
}

void moGreedyBlobTrackerModule::trackBlobs() {
    moDataGenericList::iterator it;
	double weight, min_weight;
	for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++){
        // For each blob from the new frame, find the closest one from the old frame(s)
        moDataGenericContainer* closest_blob = NULL;
        min_weight = this->properties["max_weight"]->asDouble();

        moDataGenericList::iterator it_old;
	    for (it_old = this->old_blobs->begin(); it_old != this->old_blobs->end(); it_old++){
            if ((*it_old)->properties["blob_id"]->asInteger() < 0) {
				// Blob was already assigned, i.e. the ID was already reused.
                continue;
			}

			// FIXME Make sure that our INPUT is in 0.0 - 1.0. I checked and it seemed to not always be...
			weight = this->calcWeight(*it_old, *it);

            if (weight < min_weight) {
                closest_blob = (*it_old);
                min_weight = weight;
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

