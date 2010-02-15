#ifndef OT_IMAGE_DISPLAY_MODULE_H
#define OT_IMAGE_DISPLAY_MODULE_H

#include <string>
#include "otModule.h"

class otDataStream;

class otImageDisplayModule : public otModule {
public:
	otImageDisplayModule();
	virtual ~otImageDisplayModule();

	void notifyData(otDataStream *stream);

	void setInput(otDataStream *input, int n=0);
	otDataStream* getOutput(int n=0);

private:
	otDataStream *input;
	std::string window_name;
};

#endif

