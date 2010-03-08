#include "moRoiModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(Roi, "native", "Roi filter");

moRoiModule::moRoiModule() : moImageFilterModule(){

	MODULE_INIT();

	// declare properties
	this->properties["left"] = new moProperty(20);
	this->properties["top"] = new moProperty(10);
	this->properties["width"] = new moProperty(200);
	this->properties["height"] = new moProperty(100);

}

moRoiModule::~moRoiModule() {
}

void moRoiModule::applyFilter(){
	int left = this->property("left").asInteger();
	int top = this->property("top").asInteger();
	int width = this->property("width").asInteger();
	int height = this->property("height").asInteger();
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if (left < 0) left = 0;
	if (left > src->width) left = src->width;
	if (top < 0) top = 0;
	if (top > src->height) top = src->height;
	if (width < 0) width = 0;
	if ((left + width) > src->width) width = src->width - left;
	if (height < 0) height = 0;
	if ((top + height) > src->height) height = src->height - top;
	cvSetImageROI(src, cvRect(left, top, width, height));
	cvSetImageROI(this->output_buffer, cvRect(left, top, width, height));
	cvCopy(src, this->output_buffer, NULL);
	cvResetImageROI(src);
}


