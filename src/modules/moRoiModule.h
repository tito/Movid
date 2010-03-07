#ifndef MO_ROI_MODULE_H
#define MO_ROI_MODULE_H

#include "moImageFilterModule.h"

class moRoiModule : public moImageFilterModule{
public:
	moRoiModule();
	virtual ~moRoiModule();
	
protected:
	void applyFilter();
	int size;
	MODULE_INTERNALS();
};

#endif

