#ifndef OT_FIDUCIAL_TRACKER_MODULE_H
#define OT_FIDUCIAL_TRACKER_MODULE_H

#include "../otDataGenericContainer.h"
#include "otImageFilterModule.h"

class otFiducialTrackerModule : public otImageFilterModule {
public:
	otFiducialTrackerModule();
	virtual ~otFiducialTrackerModule();
	virtual otDataStream *getOutput(int n=0);
	
protected:
	otDataGenericList fiducials;
	otDataStream *output_data;
	
	void applyFilter();	
	void allocateBuffers();
	void clearFiducials();

	void *internal;

	MODULE_INTERNALS();
};

#endif

