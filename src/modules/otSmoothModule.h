#ifndef OT_SMOOTH_MODULE_H
#define OT_SMOOTH_MODULE_H

#include "otImageFilterModule.h"

class otSmoothModule : public otImageFilterModule{

public:
	otSmoothModule();
	
protected:
	void applyFilter();
	int width, height;

	MODULE_INTERNALS();
};

#endif

