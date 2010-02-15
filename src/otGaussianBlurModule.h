#ifndef OT_GAUSSIAN_BLUR_MODULE_H
#define OT_GAUSSIAN_BLUR_MODULE_H
#include <string>
#include "cv.h"
#include "otModule.h"
#include "otDataStream.h"
// TODO next step will be to remove this and add a generic otConvolutionFilterModule
//		that takes as argument the mask size and filter kernel
class otGaussianBlurModule : public otModule{
	
public:	
	otGaussianBlurModule();
	~otGaussianBlurModule();
	
	void notifyData(otDataStream *source);
	void setInput(otDataStream* , int n=0);	
	otDataStream* getOutput( int n=0 );
	
private:
	otDataStream* input;
	otDataStream* output;
	IplImage* smoothed;
	int width, height;
};
#endif
