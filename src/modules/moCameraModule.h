#ifndef MO_CAMERA_H
#define MO_CAMERA_H

#include "../moModule.h"

class moDataStream;

class moCameraModule : public moModule {
public:
	moCameraModule(); 
	virtual ~moCameraModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);

	virtual void start();
	virtual void stop();
	virtual void update();
	virtual void poll();

private:
	void *camera;
	moDataStream *stream;


	MODULE_INTERNALS();
};

#endif

