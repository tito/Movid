#ifndef MO_SMOOTH_MODULE_H
#define MO_SMOOTH_MODULE_H

#include "moImageFilterModule.h"

class moSmoothModule : public moImageFilterModule{
public:
	moSmoothModule();
	virtual ~moSmoothModule();
	
protected:
	void applyFilter();
	int width, height;

	MODULE_INTERNALS();
};

#endif

