#ifndef MO_COMBINE_H
#define MO_COMBINE_H

#include "../moModule.h"
#include "cv.h"

class moDataStream;

class moCombineModule : public moModule {
public:
	moCombineModule();
	virtual ~moCombineModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);
	virtual void notifyData(moDataStream *input);

	void update();

private:
	moDataStream *input1;
	moDataStream *input2;
	moDataStream *output;
	IplImage *output_buffer;
	IplImage *split;

	MODULE_INTERNALS();
};

#endif

