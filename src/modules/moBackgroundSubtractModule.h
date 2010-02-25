#ifndef MO_BG_SUB__MODULE_H
#define MO_BG_SUB__MODULE_H

#include "moImageFilterModule.h"

class moBackgroundSubtractModule : public moImageFilterModule{
public:
	moBackgroundSubtractModule();
	virtual ~moBackgroundSubtractModule();

protected:
	bool recapture;
	IplImage* bg_buffer;
	
	void applyFilter();
	void allocateBuffers();
	void stop();

	MODULE_INTERNALS();
};

#endif

