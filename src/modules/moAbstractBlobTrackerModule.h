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


#ifndef MO_ABSTRACTBLOBTRACKER_MODULE_H
#define MO_ABSTRACTBLOBTRACKER_MODULE_H

#include "../moModule.h"
#include "../moDataStream.h"
#include "../moDataGenericContainer.h"
#include "cv.h"
// Needed for thread-safe increment of the static ID if we have more than
// one tracker.

class moAbstractBlobTrackerModule : public moModule {
public:
	moAbstractBlobTrackerModule();
	virtual ~moAbstractBlobTrackerModule();

	void notifyData(moDataStream *stream);
	void update();

protected:
	static int id_counter;
	moDataGenericList* new_blobs;
	moDataGenericList* old_blobs;

	moDataStream *input;
	moDataStream *output;

	virtual void trackBlobs();

	MODULE_INTERNALS();
};

#endif

