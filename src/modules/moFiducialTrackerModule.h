#ifndef MO_FIDUCIAL_TRACKER_MODULE_H
#define MO_FIDUCIAL_TRACKER_MODULE_H

#include "../moDataGenericContainer.h"
#include "moImageFilterModule.h"

class moFiducialTrackerModule : public moImageFilterModule {
public:
	moFiducialTrackerModule();
	virtual ~moFiducialTrackerModule();
	virtual moDataStream *getOutput(int n=0);
	
protected:
	otDataGenericList fiducials;
	moDataStream *output_data;
	
	void applyFilter();	
	void allocateBuffers();
	void clearFiducials();

	void *internal;

	MODULE_INTERNALS();
};

#endif

