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


#ifndef MO_BLOB_TRACKER_MODULE_H
#define MO_BLOB_TRACKER_MODULE_H

#include "../moDataGenericContainer.h"
#include "moImageFilterModule.h"
#include "cvaux.h"

class moBlobTrackerModule : public moImageFilterModule{
public:
	moBlobTrackerModule();
	virtual ~moBlobTrackerModule();
	virtual moDataStream *getOutput(int n=0);
	
protected:
	int next_id;
	CvBlobSeq* new_blobs;
	CvBlobSeq* old_blobs;
	CvBlobTrackerAuto* tracker;	
	moDataGenericList blobs;
	moDataStream *output_data;
	CvBlobTrackerAutoParam1 param;
	
	void applyFilter();	
	void allocateBuffers();
	void clearBlobs();

	MODULE_INTERNALS();
};

#endif

