#ifndef MO_DILATE_MODULE_H
#define MO_DILATE_MODULE_H

#include <string>
#include "moImageFilterModule.h"

class moDilateModule : public moImageFilterModule {
public:
	moDilateModule();
	~moDilateModule();
	
protected:
	void applyFilter();
	
	MODULE_INTERNALS();
};

#endif

