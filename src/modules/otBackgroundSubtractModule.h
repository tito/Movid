#ifndef OT_BG_SUB__MODULE_H
#define OT_BG_SUB__MODULE_H

#include "otImageFilterModule.h"

class otBackgroundSubtractModule : public otImageFilterModule{
public:
	otBackgroundSubtractModule();
	virtual ~otBackgroundSubtractModule();

protected:
	bool recapture;
	IplImage* bg_buffer;
	
	void applyFilter();
	void allocateBuffers();
	void stop();

	MODULE_INTERNALS();
};

#endif

