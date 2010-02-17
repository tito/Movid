#ifndef OT_IMAGE_FILTER_MODULE_H
#define OT_IMAGE_FILTER_MODULE_H

#include <string>
#include "cv.h"
#include "../otModule.h"
#include "../otDataStream.h"

class otImageFilterModule : public otModule {
	
public:	
	otImageFilterModule();
	~otImageFilterModule();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);
	
	virtual void notifyData(otDataStream *source);
	virtual void update();
	virtual void stop();
	
protected:
	otDataStream* input;
	otDataStream* output;
	IplImage* output_buffer;
	
	virtual void applyFilter()=0;
	virtual void allocateBuffers();
	
	bool need_update;

};
#endif
