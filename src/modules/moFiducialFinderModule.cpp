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


// check reactivision
// check http://www.openframeworks.cc/forum/viewtopic.php?t=486&highlight=fiducial
#include <stdlib.h>
#include <assert.h>
#include "moFiducialFinderModule.h"
#include "../moLog.h"
#include "cv.h"
#include "cvaux.h"

#include "libfidtrack/segment.h"
#include "libfidtrack/fidtrackX.h"


MODULE_DECLARE(FiducialFinder, "native", "Find Fiducials");


typedef struct {
	Segmenter segmenter;
	FiducialX fiducials[MAX_FIDUCIALS];
	RegionX regions[MAX_FIDUCIALS * 4];
	TreeIdMap treeidmap;
	FidtrackerX fidtrackerx;
	ShortPoint *dmap;
} fiducials_data_t;

moFiducialFinderModule::moFiducialFinderModule() : moImageFilterModule() {
	MODULE_INIT();

	this->output_data = new moDataStream("blob");
	this->declareOutput(1, &this->output_data, new moDataStreamInfo(
			"data", "blob", "Data stream with fiducial info"));

	this->internal = malloc(sizeof(fiducials_data_t));

	// change input type
	this->setInputType(0, "IplImage8");
}

moFiducialFinderModule::~moFiducialFinderModule() {
}

void moFiducialFinderModule::clearFiducials() {
	moDataGenericList::iterator it;
	for ( it = this->fiducials.begin(); it != this->fiducials.end(); it++ )
		delete (*it);
	this->fiducials.clear();
}

void moFiducialFinderModule::allocateBuffers() {
	IplImage* src = (IplImage*)(this->input->getData());
	this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, 3);	// only one channel
	LOG(MO_DEBUG, "allocated output buffer for FiducialFinder module.");

	// first time, initialize fids
	fiducials_data_t *fids = (fiducials_data_t *)this->internal;
	//initialize_treeidmap_from_file( &treeidmap, tree_config );
	initialize_treeidmap( &fids->treeidmap );

	fids->dmap = new ShortPoint[src->height*src->width];
	for ( int y = 0; y < src->height; y++ ) {
		for ( int x = 0; x < src->width; x++ ) {
			fids->dmap[y*src->width+x].x = x;
			fids->dmap[y*src->width+x].y = y;
		}
	}

	initialize_fidtrackerX( &fids->fidtrackerx, &fids->treeidmap, fids->dmap);
	initialize_segmenter( &fids->segmenter, src->width, src->height, fids->treeidmap.max_adjacencies );
}

void moFiducialFinderModule::applyFilter(IplImage *src) {
	fiducials_data_t *fids = static_cast<fiducials_data_t*>(this->internal);
	moDataGenericContainer *fiducial;
	FiducialX *fdx;
	int fid_count, valid_fiducials = 0;
	bool do_image = this->output->getObserverCount() > 0 ? true : false;
	CvSize size = cvGetSize(src);

	CvFont font, font2;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 2);
	cvInitFont(&font2, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0, 0, 1);

	assert( src != NULL );
	assert( fids != NULL );
	assert( src->imageData != NULL );

	if ( src->nChannels != 1 ) {
		this->setError("FiducialFinder input image must be a single channel binary image.");
		this->stop();
		return;
	}


	// prepare image if we have listener on output
	if ( do_image )
		cvSet(this->output_buffer, CV_RGB(0, 0, 0));

	// libfidtrack
	step_segmenter(&fids->segmenter, (const unsigned char*)src->imageData);
	fid_count = find_fiducialsX(fids->fiducials, MAX_FIDUCIALS,
			&fids->fidtrackerx, &fids->segmenter, src->width, src->height);

	// prepare to refill fiducials
	this->clearFiducials();

	for ( int i = 0; i < fid_count; i++ ) {
		fdx = &fids->fiducials[i];

		// invalid id (INVALID_FIDUCIAL_ID)
		if ( fdx->id < 0 )
			continue;

		// got a valid fiducial ! process...
		valid_fiducials++;

		LOGM(MO_DEBUG, "fid:" << i << " id=" << fdx->id << " pos=" \
			<< fdx->x << "," << fdx->y << " angle=" << fdx->angle);

		fiducial = new moDataGenericContainer();
		fiducial->properties["implements"] = new moProperty("fiducial,pos,tracked");
		fiducial->properties["fiducial_id"] = new moProperty(fdx->id);
		fiducial->properties["blob_id"] = new moProperty(fdx->id);
		fiducial->properties["x"] = new moProperty(fdx->x / size.width);
		fiducial->properties["y"] = new moProperty(fdx->y / size.height);
		fiducial->properties["angle"] = new moProperty(fdx->angle);
		fiducial->properties["leaf_size"] = new moProperty(fdx->leaf_size);
		fiducial->properties["root_size"] = new moProperty(fdx->root_size);
		this->fiducials.push_back(fiducial);

		// draw on output image
		if ( do_image ) {
			std::ostringstream oss;
			oss << fdx->id;
			cvPutText(this->output_buffer, oss.str().c_str(),
				cvPoint(fdx->x, fdx->y - 20), &font, cvScalar(20, 255, 20));

			oss.str("");
			oss << "angle:" << int(fdx->angle * 180 / 3.14159265);
			cvPutText(this->output_buffer, oss.str().c_str(),
				cvPoint(fdx->x - 30, fdx->y), &font2, cvScalar(20, 255, 20));

			oss.str("");
			oss << "l/r:" << fdx->leaf_size << "/" << fdx->root_size;
			cvPutText(this->output_buffer, oss.str().c_str(),
				cvPoint(fdx->x - 50, fdx->y + 20), &font2, cvScalar(20, 255, 20));

		}
	}

	LOGM(MO_DEBUG, "-> Found " << valid_fiducials << " fiducials");
	this->output_data->push(&this->fiducials);
}

