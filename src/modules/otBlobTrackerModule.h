#ifndef OT_BLOB_TRACKER_MODULE_H
#define OT_BLOB_TRACKER_MODULE_H

#include "../otDataGenericContainer.h"
#include "otImageFilterModule.h"
#include "cvaux.h"

class otBlobTrackerModule : public otImageFilterModule{
public:
	otBlobTrackerModule();
	virtual ~otBlobTrackerModule();
	virtual otDataStream *getOutput(int n=0);
	
protected:
	int next_id;
	CvBlobSeq* new_blobs;
	CvBlobSeq* old_blobs;
	CvBlobTrackerAuto* tracker;	
	otDataGenericList blobs;
	otDataStream *output_data;
	
	void applyFilter();	
	void clearBlobs();

	MODULE_INTERNALS();
};

#endif

