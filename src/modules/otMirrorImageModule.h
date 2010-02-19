#ifndef OT_MIRROR_IMAGE_MODULE_H
#define OT_MIRROR_IMAGE_MODULE_H

#include "otImageFilterModule.h"

class otMirrorImageModule : public otImageFilterModule{
public:
	otMirrorImageModule();
	virtual ~otMirrorImageModule();
	
protected:
	void applyFilter();
	int width, height;
	
	MODULE_INTERNALS();
};

#endif

