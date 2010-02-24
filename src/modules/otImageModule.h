#ifndef OT_IMAGE_H
#define OT_IMAGE_H

#include "../otModule.h"

class otDataStream;

class otImageModule : public otModule {
public:
	otImageModule(); 
	virtual ~otImageModule();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);

	void start();
	void stop();
	void update();

private:
	void *image;
	otDataStream *stream;


	MODULE_INTERNALS();
};

#endif


