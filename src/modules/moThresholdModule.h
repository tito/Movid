#ifndef MO_THRESHOLD_MODULE_H
#define MO_THRESHOLD_MODULE_H

#include "moImageFilterModule.h"

class moThresholdModule : public moImageFilterModule{
public:
	moThresholdModule();
	virtual ~moThresholdModule();
	
protected:
	void applyFilter();
	double threshold;
	
	MODULE_INTERNALS();
};

#endif

