#ifndef OT_CAMERA_H
#define OT_CAMERA_H

#include "otModule.h"

class otDataStream;

class otCamera : public otModule {
public:
	otCamera(); 
	~otCamera();

	void start();
	void stop();
	
	void update();

private:
	void *camera;
	otDataStream *stream;
};

#endif
