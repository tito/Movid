#ifndef OT_HIGH_PASS_MODULE_H
#define OT_HIGH_PASS_MODULE_H

#include "otImageFilterModule.h"

class otHighpassModule : public otImageFilterModule{
public:
	otHighpassModule();
	virtual ~otHighpassModule();
	
protected:
	void applyFilter();
	int size;
	MODULE_INTERNALS();
};

#endif

