#ifndef MO_GPU_AMPLIFY_MODULE_H
#define MO_GPU_AMPLIFY_MODULE_H

#include "../moImageFilterModule.h"
#include "gpu_amplify.cu"

class moGPUAmplifyModule : public moImageFilterModule{
public:
	moGPUAmplifyModule();
	virtual ~moGPUAmplifyModule();

protected:
	void applyFilter(IplImage *);
	MODULE_INTERNALS();

};

#endif
