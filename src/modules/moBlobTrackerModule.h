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

