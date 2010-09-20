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
#include "../moUtils.h"

MODULE_DECLARE(GreedyBlobTracker, "native", "Track Blobs based on a simple greedy algorithm");

moGreedyBlobTrackerModule::moGreedyBlobTrackerModule() : moAbstractBlobTrackerModule() {
	MODULE_INIT();

	this->properties["max_distance"] = new moProperty(0.1);
}

void moGreedyBlobTrackerModule::trackBlobs() {
	moDataGenericList::iterator it, it_old;
	double distance, min_distance, old_x, old_y, new_x, new_y;
	int old_fid, fid, old_id;
	moDataGenericContainer* closest_blob;
	std::string implements, old_implements;
	bool old_is_fid, new_is_fid;
	this->reused.clear();

	for (it = this->new_blobs->begin(); it != this->new_blobs->end(); it++){
		// For each blob from the new frame, find the closest one from the old frame(s)
		closest_blob = NULL;
		min_distance = this->properties["max_distance"]->asDouble();

		new_x = (*it)->properties["x"]->asDouble();
		new_y = (*it)->properties["y"]->asDouble();
		implements = (*it)->properties["implements"]->asString();
		new_is_fid = moUtils::inList("fiducial", implements) || moUtils::inList("markerlessobject", implements);

		for (it_old = this->old_blobs->begin(); it_old != this->old_blobs->end(); it_old++){
			old_id = (*it_old)->properties["blob_id"]->asInteger();
			if (std::count(this->reused.begin(), this->reused.end(), old_id))
				// Blob was already assigned, i.e. the ID was already reused.
				continue;

			old_implements = (*it_old)->properties["implements"]->asString();
			old_is_fid = moUtils::inList("fiducial", old_implements) || moUtils::inList("markerlessobject", old_implements);
			if (old_is_fid && new_is_fid) {
				fid = (*it)->properties["fiducial_id"]->asInteger();
				old_fid = (*it_old)->properties["fiducial_id"]->asInteger();
				if (fid != old_fid)
					// This just can't be the same blob if it got a different fiducial ID.
					continue;
			}

			if ((old_is_fid && !new_is_fid) || (!old_is_fid && new_is_fid))
				// If either is a fiducial while the other isn't, there can't be a possible match.
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
		if (closest_blob) {
			old_id = closest_blob->properties["blob_id"]->asInteger();
			(*it)->properties["blob_id"]->set( old_id );
			// Indicate that we reused this blob already and that it shouldn't be considered anymore
			this->reused.push_back(old_id);
		}
		//this must be a new blob, so assign new ID
		else
			// Atomically increment counter in case we got more than one tracker
			(*it)->properties["blob_id"]->set(pt::pincrement(&(moAbstractBlobTrackerModule::id_counter)));
	}
}

