#ifndef MO_MIRROR_IMAGE_MODULE_H
#define MO_MIRROR_IMAGE_MODULE_H

#include "moImageFilterModule.h"

class moMirrorImageModule : public moImageFilterModule{
public:
	moMirrorImageModule();
	virtual ~moMirrorImageModule();
	
protected:
	void applyFilter();
	int width, height;
	
	MODULE_INTERNALS();
};

#endif

