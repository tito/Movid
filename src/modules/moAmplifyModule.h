#ifndef MO_AMPLIFY_MODULE_H
#define MO_AMPLIFY_MODULE_H

#include "moImageFilterModule.h"

class moAmplifyModule : public moImageFilterModule{
public:
	moAmplifyModule();
	virtual ~moAmplifyModule();
	
protected:
	void applyFilter();	
	MODULE_INTERNALS();
};

#endif

