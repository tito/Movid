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
#include "moBlobFinderModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(BlobFinder, "native", "ContourBlobs Description");

moBlobFinderModule::moBlobFinderModule() : moImageFilterModule(){

	MODULE_INIT();

	this->storage = cvCreateMemStorage(0);

	this->output_data = new moDataStream("GenericBlob");
	this->output_count = 2;
	this->output_infos[1] = new moDataStreamInfo("data", "GenericBlob", "Data stream with Blob info");
	this->blobs = new moDataGenericList();
}

moBlobFinderModule::~moBlobFinderModule() {
	cvReleaseMemStorage(&this->storage);
	delete this->blobs;
}

void moBlobFinderModule::clearBlobs() {
	moDataGenericList::iterator it;
	for ( it = this->blobs->begin(); it != this->blobs->end(); it++ )
		delete (*it);
	this->blobs->clear();
}

void moBlobFinderModule::applyFilter(IplImage *src) {

	this->clearBlobs();
	cvCopy(src, this->output_buffer);
	
    CvSeq *contours = 0;
	cvFindContours(this->output_buffer, this->storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
	if (!contours)
		return;

    cvDrawContours(this->output_buffer, contours, cvScalarAll(255), cvScalarAll(255), 100);

	// Consider each contour a blob and extract the blob infos from it.
	CvSeq *cur_cont = contours;
	while (cur_cont != 0) {
		CvRect rect	= cvBoundingRect(cur_cont, 0);

		moDataGenericContainer *blob = new moDataGenericContainer();
		blob->properties["type"] = new moProperty("blob");
		blob->properties["x"] = new moProperty(rect.x + rect.width / 2);
		blob->properties["y"] = new moProperty(rect.y + rect.height / 2);
		blob->properties["width"] = new moProperty(rect.width);
        blob->properties["height"] = new moProperty(rect.height);
        blobs->push_back(blob);

		cur_cont = cur_cont->h_next;
	}
	
    this->output_data->push(blobs);
}

moDataStream* moBlobFinderModule::getOutput(int n) {
	if ( n == 1 )
		return this->output_data;
	return moImageFilterModule::getOutput(n);
}


