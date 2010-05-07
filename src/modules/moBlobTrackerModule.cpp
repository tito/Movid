/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#include <assert.h>
#include "moBlobTrackerModule.h"
#include "../moLog.h"
#include "cv.h"
#include "cvaux.h"


//pass through f detection
class moFGDetector : public CvFGDetector{
private:
	IplImage* blob_image;

public:
    IplImage* GetMask();
    void    Process(IplImage* pImg);
	void    Release();
};

IplImage* moFGDetector::GetMask(){
	return this->blob_image;
}
void moFGDetector::Process(IplImage* pImg){
	assert ("Blob Tracker input image, must be single channel binary image!" && pImg->nChannels ==1);
	this->blob_image = pImg;
}
void moFGDetector::Release(){

}




 
//class moBlobFinder : CvBlobDetector
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

//moBlobFinder::DetectNewBlob((IplImage* pFGMask, CvBlobSeq* pNewBlobList, CvBlobSeq* pOldBlobList)){
//}

//moBlobFinder::Release();





MODULE_DECLARE(BlobTracker, "native", "Tracks Blobs");

moBlobTrackerModule::moBlobTrackerModule() : moImageFilterModule() {
	MODULE_INIT();

	// Minimum and maximum sizes for blobs. Blobs smaller or larger
	// will be discarded.
	this->properties["min_size"] = new moProperty(2.0);
	this->properties["max_size"] = new moProperty(25.0);

	this->output_data	= new moDataStream("GenericTouch");
	this->output_count	= 2;
	this->output_infos[1] = new moDataStreamInfo("data", "GenericTouch", "Data stream with touch info");

	this->next_id		= 1;
	this->new_blobs		= new CvBlobSeq();
	this->old_blobs		= new CvBlobSeq();

	memset(&this->param, 0, sizeof(CvBlobTrackerAutoParam1));
	this->param.FGTrainFrames = 0;
	this->param.pFG		= new moFGDetector();//cvCreateFGDetectorBase(CV_BG_MODEL_FGD, NULL); //new moFGDetector();
	this->param.pBT		= cvCreateBlobTrackerCCMSPF();
	this->param.pBTPP	= cvCreateModuleBlobTrackPostProcKalman();
	this->tracker		= cvCreateBlobTrackerAuto1(&this->param);
}

moBlobTrackerModule::~moBlobTrackerModule() {
	this->clearBlobs();
	delete this->output_data;
	delete this->new_blobs;
	delete this->old_blobs;

	// blob track release ?
	delete this->param.pFG;
	cvReleaseBlobTrackPostProc(&this->param.pBTPP);
	cvReleaseBlobTracker(&this->param.pBT);
	cvReleaseBlobTrackerAuto(&this->tracker);
	delete this->tracker;
}

void moBlobTrackerModule::clearBlobs() {
	moDataGenericList::iterator it;
	for ( it = this->blobs.begin(); it != this->blobs.end(); it++ )
		delete (*it);
	this->blobs.clear();
}


void moBlobTrackerModule::allocateBuffers() {
	IplImage* src = static_cast<IplImage*>(this->input->getData());
	if ( src == NULL )
		return;
	this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, 3);
	LOGM(MO_TRACE) << "allocated output buffer for BlobTracker module.";
}

void moBlobTrackerModule::applyFilter(IplImage *src) {
	IplImage* fg_map = NULL;

	assert( src != NULL );
	CvSize size = cvGetSize(src);

	if ( src->nChannels != 1 ) {
		this->setError("BlobTracker input image must be a single channel binary image.");
		this->stop();
		return;
	}

	this->tracker->Process(src, fg_map);

	cvSet(this->output_buffer, CV_RGB(0,0,0));
	this->clearBlobs();

	for ( int i = this->tracker->GetBlobNum(); i > 0; i-- ) {
		CvBlob* pB = this->tracker->GetBlob(i-1);

		int minsize = this->property("min_size").asDouble();
		int maxsize = this->property("max_size").asDouble();
		// Assume circular blobs
		if (pB->w < minsize || maxsize < pB->w || pB->h < minsize || maxsize < pB->h)
			continue;
		// draw the blob on output image
		if ( this->output->getObserverCount() > 0 ) {
			CvPoint p = cvPoint(cvRound(pB->x*256),cvRound(pB->y*256));
			CvSize  s = cvSize(MAX(1,cvRound(CV_BLOB_RX(pB)*256)), MAX(1,cvRound(CV_BLOB_RY(pB)*256)));
			int c = cvRound(255*this->tracker->GetState(CV_BLOB_ID(pB)));

			cvEllipse(this->output_buffer, p, s, 0, 0, 360,
				CV_RGB(c,255-c,0), cvRound(1+(3*0)/255), CV_AA, 8);
		}

		LOGM(MO_DEBUG) << "Blob: id="<< pB->ID <<" pos=" << pB->x \
			<< "," << pB->y << "size=" << pB->w << "," << pB->h;

		// add the blob in data
		moDataGenericContainer *touch = new moDataGenericContainer();
		touch->properties["type"] = new moProperty("touch");
		touch->properties["id"] = new moProperty(pB->ID);
		touch->properties["x"] = new moProperty(pB->x / size.width);
		touch->properties["y"] = new moProperty(pB->y / size.height);
		touch->properties["w"] = new moProperty(pB->w);
		touch->properties["h"] = new moProperty(pB->h);
		this->blobs.push_back(touch);
	};

	this->output_data->push(&this->blobs);
}

moDataStream* moBlobTrackerModule::getOutput(int n) {
	if ( n == 1 )
		return this->output_data;
	return moImageFilterModule::getOutput(n);
}

