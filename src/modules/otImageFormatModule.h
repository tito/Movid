#ifndef OT_IMG_FORMAT__MODULE_H
#define OT_IMG_FORMAT__MODULE_H

#include "otImageFilterModule.h"

class otImageFormatModule : public otImageFilterModule{
public:
	otImageFormatModule();
	virtual ~otImageFormatModule();
	
protected:	
	void applyFilter();
	void allocateBuffers();
	
	MODULE_INTERNALS();
};

#endif

