#ifndef OT_IMAGE_DISPLAY_MODULE_H
#define OT_IMAGE_DISPLAY_MODULE_H

#include "otModule.h"

class otDataStream;

class otImageDisplayModule : public otModule {
public:
	otImageDisplayModule(const char *name);
	virtual ~otImageDisplayModule();

	void update(otDataStream *input);

private:
	otDataStream *input;
	std::string window_name;
};

#endif

