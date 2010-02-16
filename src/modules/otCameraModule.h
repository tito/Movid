#ifndef OT_CAMERA_H
#define OT_CAMERA_H

#include "../otModule.h"

class otDataStream;

class otCameraModule : public otModule {
public:
	otCameraModule(); 
	~otCameraModule();

	void setInput(otDataStream *input, int n=0);
	otDataStream* getOutput(int n=0);

	void start();
	void stop();

	void update();

private:
	void *camera;
	otDataStream *stream;
};

#endif
