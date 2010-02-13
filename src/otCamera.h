#include "cv.h"
#include "highgui.h"

class otCamera {

public:
	otCamera(int=0); 
	~otCamera();
	
	IplImage* getFrame();	
	void      update();

private:
	CvCapture* cam_capture;
	IplImage*  cam_frame;

};