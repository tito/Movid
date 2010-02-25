#ifndef MO_HIGH_PASS_MODULE_H
#define MO_HIGH_PASS_MODULE_H

#include "moImageFilterModule.h"

class moHighpassModule : public moImageFilterModule{
public:
	moHighpassModule();
	virtual ~moHighpassModule();
	
protected:
	void applyFilter();
	int size;
	MODULE_INTERNALS();
};

#endif

