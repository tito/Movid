#ifndef OT_AMPLIFY_MODULE_H
#define OT_AMPLIFY_MODULE_H

#include "otImageFilterModule.h"
#include "cvaux.h"

class otBlobTrackerModule : public otImageFilterModule{
public:
	otBlobTrackerModule();
	virtual ~otBlobTrackerModule();
	
protected:
	void applyFilter();	
	CvBlobSeq* new_blobs;
	CvBlobSeq* old_blobs;
	
	CvBlobTrackerAuto* tracker;
	CvFGDetector*   fg_detector;
	CvBlobDetector* blob_detector;
	CvBlobTracker*  blob_tracker;
	
	int next_id;
	
	MODULE_INTERNALS();
};

#endif

