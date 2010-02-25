#ifndef MO_ERODE_MODULE_H
#define MO_ERODE_MODULE_H

#include <string>
#include "moImageFilterModule.h"

class moErodeModule : public moImageFilterModule {
public:
	moErodeModule();
	~moErodeModule();
	
protected:
	void applyFilter();
	
	MODULE_INTERNALS();
};

#endif

