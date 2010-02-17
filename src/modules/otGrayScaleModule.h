#ifndef OT_GRAY_SCALE_MODULE_H
#define OT_GRAY_SCALE_MODULE_H

#include "otImageFilterModule.h"

class otGrayScaleModule : public otImageFilterModule{
public:
	otGrayScaleModule();
	virtual ~otGrayScaleModule();
	
protected:
	void applyFilter();
	void allocateBuffers();
	MODULE_INTERNALS();
};

#endif

