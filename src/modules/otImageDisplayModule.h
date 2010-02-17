#ifndef OT_IMAGE_DISPLAY_MODULE_H
#define OT_IMAGE_DISPLAY_MODULE_H

#include <string>
#include "../otModule.h"

class otDataStream;

class otImageDisplayModule : public otModule {
public:
	otImageDisplayModule();
	virtual ~otImageDisplayModule();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);

	void notifyData(otDataStream *stream);
	void update();

	void stop();

private:
	otDataStream *input;
	std::string window_name;

	MODULE_INTERNALS();
};

#endif

