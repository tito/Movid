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


#include "moMunkresBlobTrackerModule.h"
#include "../moLog.h"

#define NORMALIZATION_FACTOR 10000.

MODULE_DECLARE(MunkresBlobTracker, "native", "Track Blobs based on munkres algorithm.");

moMunkresBlobTrackerModule::moMunkresBlobTrackerModule() : moAbstractBlobTrackerModule() {
	MODULE_INIT();
}

void moMunkresBlobTrackerModule::trackBlobs() {
	int A[DIM2_FINGER], *row;
	int n2s[DIM_FINGER];
	int id, i, j;

	int sn = this->old_blobs->size();
	int nn = this->new_blobs->size();

	assert(sn < DIM_FINGER);
	assert(nn < DIM_FINGER);

	int sid[DIM_FINGER];
	// The munkres algorithm expects integers since it's doing some optimizations.
	// Hence, we need to convert our internal doubles to ints later on.
	int nx[DIM_FINGER];
	int ny[DIM_FINGER];
	int sx[DIM_FINGER];
	int sy[DIM_FINGER];

	for (i = 0; i < sn; i++) {
		sid[i] = (*this->old_blobs)[i]->properties["blob_id"]->asInteger();
		// Normalize and put them into the int (!) array
		sx[i] = (*this->old_blobs)[i]->properties["x"]->asDouble() * NORMALIZATION_FACTOR;
		sy[i] = (*this->old_blobs)[i]->properties["y"]->asDouble() * NORMALIZATION_FACTOR;
	}
	for (i = 0; i < nn; i++) {
		// Normalize and put them into the int (!) array
		nx[i] = (*this->new_blobs)[i]->properties["x"]->asDouble() * NORMALIZATION_FACTOR;
		ny[i] = (*this->new_blobs)[i]->properties["y"]->asDouble() * NORMALIZATION_FACTOR;
	}
	
	// Setup distance matrix for contact matching.
	// The algorithm uses the integer values.
	for (j = 0; j < sn; j++) {
		row = A + nn * j;
		for (i = 0; i < nn; i++) {
			row[i] = dist2(nx[i] - sx[j], ny[i] - sy[j]);
		}
	}

	mtdev_match(n2s, A, nn, sn);

	for (i = 0; i < nn; i++) {
		j = n2s[i];
		id = j >= 0 ? sid[j] : -10;
		if (id == -10)
			id = this->id_counter++;
		// No need to undo the normalization since we never changed the actual blob.
		(*this->new_blobs)[i]->properties["blob_id"]->set(id);
	}
}

