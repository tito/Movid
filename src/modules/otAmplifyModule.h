#ifndef OT_AMPLIFY_MODULE_H
#define OT_AMPLIFY_MODULE_H

#include "otImageFilterModule.h"

class otAmplifyModule : public otImageFilterModule{
public:
	otAmplifyModule();
	virtual ~otAmplifyModule();
	
protected:
	void applyFilter();	
	MODULE_INTERNALS();
};

#endif

