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

MODULE_DECLARE(MunkresBlobTracker, "native", "Track Blobs based on munkres algorithm.");

moMunkresBlobTrackerModule::moMunkresBlobTrackerModule() : moAbstractBlobTrackerModule() {
	MODULE_INIT();
}

double moMunkresBlobTrackerModule::calcWeight(moDataGenericContainer* old_blob,
											 moDataGenericContainer* new_blob) {
	return 1.0;
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
	int nx[DIM_FINGER];
	int ny[DIM_FINGER];
	int sx[DIM_FINGER];
	int sy[DIM_FINGER];

	std::cout << "\n\n============ OLD BLOBS ==============" << std::endl;
	for (i = 0; i < sn; i++) {
		sid[i] = (*this->old_blobs)[i]->properties["blob_id"]->asInteger();
		sx[i] = (*this->old_blobs)[i]->properties["x"]->asDouble();
		sy[i] = (*this->old_blobs)[i]->properties["y"]->asDouble();
		std::cout << "j: " << i << " ID " << sid[i] << " x " << sx[i] << " y " << sy[i] << std::endl;
	}
	std::cout << "\n============ NEW BLOBS ==============" << std::endl;
	for (i = 0; i < nn; i++) {
		std::cout << "i: " << i << " x " << nx[i] << " y " << ny[i] << std::endl;
		nx[i] = (*this->new_blobs)[i]->properties["x"]->asDouble();
		ny[i] = (*this->new_blobs)[i]->properties["y"]->asDouble();
	}
	
	// setup distance matrix for contact matching
	std::cout << "\n============ DISTANCES ==============" << std::endl;
	for (j = 0; j < sn; j++) {
		row = A + nn * j;
		for (i = 0; i < nn; i++) {
			row[i] = dist2(nx[i] - sx[j], ny[i] - sy[j]);
			std::cout << j << " to " << i << ": " << row[i] << std::endl;
		}
	}

	mtdev_match(n2s, A, nn, sn);

	std::cout << "\n============ MATCHING ==============" << std::endl;
	for (i = 0; i < nn; i++) {
		j = n2s[i];
		id = j >= 0 ? sid[j] : -10;
		if (id == -10)
			id = this->id_counter++;
		(*this->new_blobs)[i]->properties["blob_id"]->set(id);
	}
}

