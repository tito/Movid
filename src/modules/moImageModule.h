#ifndef MO_IMAGE_H
#define MO_IMAGE_H

#include "../moModule.h"

class moDataStream;

class moImageModule : public moModule {
public:
	moImageModule(); 
	virtual ~moImageModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);

	void start();
	void stop();
	void update();

private:
	void *image;
	moDataStream *stream;


	MODULE_INTERNALS();
};

#endif


