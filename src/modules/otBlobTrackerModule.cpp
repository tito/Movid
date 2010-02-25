#include <assert.h>
#include "otBlobTrackerModule.h"
#include "cv.h"
#include "cvaux.h"


//pass through f detection
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




 
//class otBlobFinder : CvBlobDetector
//{
//public:
    /* pFGMask - image of foreground mask */
    /* pNewBlobList - pointer to sequence to save new detected blobs  */
    /* pOldBlobList - pointer to blob list which already exist on image */
	/* return number of detected blobs */
//	int DetectNewBlob(IplImage* pFGMask, CvBlobSeq* pNewBlobList, CvBlobSeq* pOldBlobList) = 0;
	
    /* release blob detector */
//    void Release()=0;
//};

//otBlobFinder::DetectNewBlob((IplImage* pFGMask, CvBlobSeq* pNewBlobList, CvBlobSeq* pOldBlobList)){
//}

//otBlobFinder::Release();





MODULE_DECLARE(BlobTracker, "native", "Tracks Blobs");

otBlobTrackerModule::otBlobTrackerModule() : otImageFilterModule() {
	MODULE_INIT();

	this->output_data = new otDataStream("otDataGenericList");
	this->output_count = 2;
	this->output_infos[1] = new otDataStreamInfo("data", "otDataGenericList", "Data stream with touch container");

	this->next_id = 1;
	this->new_blobs = new CvBlobSeq();
	this->old_blobs = new CvBlobSeq();

	CvBlobTrackerAutoParam1 param = {0};
	param.FGTrainFrames = 0;
	param.pFG       = new otFGDetector();//cvCreateFGDetectorBase(CV_BG_MODEL_FGD, NULL); //new otFGDetector();
	param.pBT       = cvCreateBlobTrackerCCMSPF();
	param.pBTPP     = cvCreateModuleBlobTrackPostProcKalman();
	this->tracker = cvCreateBlobTrackerAuto1(&param);
}

otBlobTrackerModule::~otBlobTrackerModule() {
}

void otBlobTrackerModule::clearBlobs() {
	otDataGenericList::iterator it;
	for ( it = this->blobs.begin(); it != this->blobs.end(); it++ )
		delete (*it);
	this->blobs.clear();
}


void otBlobTrackerModule::allocateBuffers(){
	IplImage* src = (IplImage*)(this->input->getData());
	this->output_buffer = cvCreateImage(cvGetSize(src),src->depth, 3);	//only one channel
	LOG(DEBUG) << "allocated output buffer for BlobTracker module.";
}

void otBlobTrackerModule::applyFilter() {
	IplImage* src = (IplImage*)(this->input->getData());
	IplImage* fg_map = NULL;
	assert( src != NULL );

	this->tracker->Process(src, fg_map);
	
	cvSet(this->output_buffer, CV_RGB(0,0,0));
	this->clearBlobs();

	for ( int i = this->tracker->GetBlobNum(); i > 0; i-- ) {

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

		// Not optimized, but will not care for today.
		otDataGenericContainer *touch = new otDataGenericContainer();
		touch->properties["type"] = new otProperty("touch");
		touch->properties["id"] = new otProperty(pB->ID);
		touch->properties["x"] = new otProperty(pB->x);
		touch->properties["y"] = new otProperty(pB->y);
		touch->properties["w"] = new otProperty(pB->w);
		touch->properties["h"] = new otProperty(pB->h);
		this->blobs.push_back(touch);
	}   /* Next blob. */;

	this->output_data->push(&this->blobs);
}

otDataStream* otBlobTrackerModule::getOutput(int n) {
	if ( n == 1 )
		return this->output_data;
	return otImageFilterModule::getOutput(n);
}

