#ifndef OT_BLOB_TRACKER_MODULE_H
#define OT_BLOB_TRACKER_MODULE_H

#include "otImageFilterModule.h"
#include "cvaux.h"

class otBlobTrackerModule : public otImageFilterModule{
public:
	otBlobTrackerModule();
	virtual ~otBlobTrackerModule();
	
protected:
	int next_id;
	CvBlobSeq* new_blobs;
	CvBlobSeq* old_blobs;
	CvBlobTrackerAuto* tracker;	
	
	void applyFilter();	
	MODULE_INTERNALS();
};

#endif

