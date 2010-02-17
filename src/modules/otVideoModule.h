#ifndef OT_VIDEO_MODULE_H
#define OT_VIDEO_MODULE_H

#include "../otModule.h"

class otVideoModule : public otModule {
public:
	otVideoModule();
	virtual ~otVideoModule();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);

	void start();
	void stop();
	void update();

private:
	otDataStream *stream;
	int numframes;
	void *video;

	MODULE_INTERNALS();
};

#endif

