#ifndef MO_IMAGE_FILTER_MODULE_H
#define MO_IMAGE_FILTER_MODULE_H

#include <string>
#include "cv.h"
#include "../moModule.h"
#include "../moDataStream.h"

class moImageFilterModule : public moModule {
	
public:	
	moImageFilterModule();
	~moImageFilterModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);
	
	virtual void notifyData(moDataStream *source);
	virtual void update();
	virtual void stop();
	
protected:
	moDataStream* input;
	moDataStream* output;
	IplImage* output_buffer;
	
	virtual void applyFilter()=0;
	virtual void allocateBuffers();
	
	bool need_update;

};
#endif
