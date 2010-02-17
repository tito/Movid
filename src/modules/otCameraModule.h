#ifndef OT_CAMERA_H
#define OT_CAMERA_H

#include "../otModule.h"

class otDataStream;

class otCameraModule : public otModule {
public:
	otCameraModule(); 
	virtual ~otCameraModule();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);

	void start();
	void stop();
	void update();

private:
	void *camera;
	otDataStream *stream;


	MODULE_INTERNALS();
};

#endif

