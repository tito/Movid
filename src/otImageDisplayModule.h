#ifndef OT_IMAGE_DISPLAY_MODULE_H
#define OT_IMAGE_DISPLAY_MODULE_H

#include <string>
#include "otModule.h"
#include "otImageDataStream.h"


class otImageDisplayModule : public otModule {
	
public:	
	otImageDisplayModule(char* name);
	~otImageDisplayModule();
	
	void update();
	void setInput( otDataStream* , int n=0);	
	otDataStream* getOutput( int n=0 );
	
private:
	otImageDataStream* input;
	std::string* window_name;
	
};

#endif