#ifndef OT_VIDEO_MODULE_H
#define OT_VIDEO_MODULE_H

#include "../otModule.h"

class otVideoModule : public otModule {
public:
	otVideoModule();
	virtual ~otVideoModule();

	void start();
	void stop();
	void update();
	void setInput(otDataStream* , int n=0);	
	otDataStream* getOutput( int n=0 );

private:
	otDataStream *stream;
	int numframes;
	void *video;

	MODULE_INTERNALS();
};

#endif

