#ifndef OT_IMAGE_DATA_STREAM_H
#define OT_IMAGE_DATA_STREAM_H

#include "cv.h"
#include "otDataStream.h"

class otImageDataStream : public otDataStream {
	
public:	
	void setImageData(IplImage* data);
	IplImage* getImageData();
	
protected:
	IplImage* image;
};

#endif

