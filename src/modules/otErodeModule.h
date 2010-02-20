#ifndef OT_ERODE_MODULE_H
#define OT_ERODE_MODULE_H

#include <string>
#include "otImageFilterModule.h"

class otErodeModule : public otImageFilterModule {
public:
	otErodeModule();
	~otErodeModule();
	
protected:
	void applyFilter();
	
	MODULE_INTERNALS();
};

#endif

