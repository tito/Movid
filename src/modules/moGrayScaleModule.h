#ifndef MO_GRAY_SCALE_MODULE_H
#define MO_GRAY_SCALE_MODULE_H

#include "moImageFilterModule.h"

class moGrayScaleModule : public moImageFilterModule{
public:
	moGrayScaleModule();
	virtual ~moGrayScaleModule();
	
protected:
	void applyFilter();
	void allocateBuffers();
	MODULE_INTERNALS();
};

#endif

