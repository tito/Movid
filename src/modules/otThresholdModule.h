#ifndef OT_THRESHOLD_MODULE_H
#define OT_THRESHOLD_MODULE_H

#include "otImageFilterModule.h"

class otThresholdModule : public otImageFilterModule{
public:
	otThresholdModule();
	virtual ~otThresholdModule();
	
protected:
	void applyFilter();
	double threshold;
	
	MODULE_INTERNALS();
};

#endif

