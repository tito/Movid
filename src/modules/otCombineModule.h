#ifndef OT_COMBINE_H
#define OT_COMBINE_H

#include "../otModule.h"
#include "cv.h"

class otDataStream;

class otCombineModule : public otModule {
public:
	otCombineModule();
	virtual ~otCombineModule();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);
	virtual void notifyData(otDataStream *input);

	void stop();
	void update();

private:
	otDataStream *input1;
	otDataStream *input2;
	otDataStream *output;
	IplImage *output_buffer;
	bool need_update;

	MODULE_INTERNALS();
};

#endif

