#include <assert.h>
#include "otBlobTrackerModule.h"
#include "cv.h"
#include "cvaux.h"



class otFGDetector : public CvFGDetector{
private:
	IplImage* blob_image;

public:
    IplImage* GetMask();
    void    Process(IplImage* pImg);
	void    Release();
};


IplImage* otFGDetector::GetMask(){
	return this->blob_image;
}

void otFGDetector::Process(IplImage* pImg){
	assert ("Blob Tracker input image, must be single channel binary image!" && pImg->nChannels ==1);
	this->blob_image = pImg;
}

void otFGDetector::Release(){

}



MODULE_DECLARE(BlobTracker, "native", "Tracks Blobs");

otBlobTrackerModule::otBlobTrackerModule() : otImageFilterModule(){
	MODULE_INIT();
	
	this->next_id = 1;
	this->new_blobs = new CvBlobSeq();
	this->old_blobs = new CvBlobSeq();

	CvBlobTrackerAutoParam1 param = {0};
	param.FGTrainFrames = 0;
	param.pFG       = cvCreateFGDetectorBase(CV_BG_MODEL_FGD, NULL); //new otFGDetector();
	param.pBD       = cvCreateBlobDetectorCC();
	param.pBT       = cvCreateBlobTrackerCCMSPF();
	param.pBTGen    = NULL;
	param.pBTPP     = cvCreateModuleBlobTrackPostProcKalman();
	param.UsePPData = false;
	param.pBTA      = NULL;

	this->tracker = cvCreateBlobTrackerAuto1(&param);
}

otBlobTrackerModule::~otBlobTrackerModule() {

}


void otBlobTrackerModule::applyFilter(){
	IplImage* src = (IplImage*)(this->input->getData());
	IplImage* fg_map = NULL;
	
	
	
	this->tracker->Process(src, fg_map);

	cvSet(this->output_buffer, CV_RGB(0,0,0));
	for(int i=this->tracker->GetBlobNum(); i>0; i--)
	{
		CvBlob* pB = this->tracker->GetBlob(i-1);
		CvPoint p = cvPoint(cvRound(pB->x*256),cvRound(pB->y*256));
		CvSize  s = cvSize(MAX(1,cvRound(CV_BLOB_RX(pB)*256)), MAX(1,cvRound(CV_BLOB_RY(pB)*256)));
		int c = cvRound(255*this->tracker->GetState(CV_BLOB_ID(pB)));
		
		cvEllipse( this->output_buffer,
				  p,
				  s,
				  0, 0, 360,
				  CV_RGB(c,255-c,0), cvRound(1+(3*0)/255), CV_AA, 8 );
		
		LOG(INFO) << "<Blob>:  id="<<pB->ID<<"  pos=" <<pB->x <<","<< pB->y <<"size="<<pB->w <<","<< pB->h;
	}   /* Next blob. */;
	
	


	
	
}
