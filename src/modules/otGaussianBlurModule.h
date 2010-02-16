#ifndef OT_GAUSSIAN_BLUR_MODULE_H
#define OT_GAUSSIAN_BLUR_MODULE_H
#include <string>
#include "otImageFilterModule.h"


class otGaussianBlurModule : public otImageFilterModule{

public:
	otGaussianBlurModule();
	
protected:
	void applyFilter();
	int width, height;
};
#endif
