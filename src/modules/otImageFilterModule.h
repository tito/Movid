#ifndef OT_IMAGE_FILTER_MODULE_H
#define OT_IMAGE_FILTER_MODULE_H
#include <string>
#include "cv.h"
#include "../otModule.h"
#include "../otDataStream.h"


class otImageFilterModule : public otModule{
	
public:	
	otImageFilterModule();
	~otImageFilterModule();
	
	void notifyData(otDataStream *source);
	void setInput(otDataStream* , int n=0);	
	otDataStream* getOutput( int n=0 );
	void update();
	
protected:
	otDataStream* input;
	otDataStream* output;
	IplImage* output_buffer;
	
	virtual void applyFilter()=0;
	
private:
	bool need_update;

};
#endif
