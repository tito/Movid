#ifndef OT_DILATE_MODULE_H
#define OT_DILATE_MODULE_H

#include <string>
#include "otImageFilterModule.h"

class otDilateModule : public otImageFilterModule {
public:
	otDilateModule();
	~otDilateModule();
	
protected:
	void applyFilter();
	
	MODULE_INTERNALS();
};

#endif

