#ifndef MO_IMAGE_DISPLAY_MODULE_H
#define MO_IMAGE_DISPLAY_MODULE_H

#include <string>
#include "../moModule.h"

#include "cv.h"

class moDataStream;

class moImageDisplayModule : public moModule {
public:
	moImageDisplayModule();
	virtual ~moImageDisplayModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);

	void notifyData(moDataStream *stream);
	void update();

	void stop();

private:
	moDataStream *input;
	IplImage *img;
	std::string window_name;

	MODULE_INTERNALS();
};

#endif

