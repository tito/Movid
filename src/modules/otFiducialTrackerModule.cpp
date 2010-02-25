// check reactivision
// check http://www.openframeworks.cc/forum/viewtopic.php?t=486&highlight=fiducial
#include <stdlib.h>
#include <assert.h>
#include "otFiducialTrackerModule.h"
#include "cv.h"
#include "cvaux.h"

#include "libfidtrack/segment.h"
#include "libfidtrack/fidtrackX.h"


MODULE_DECLARE(FiducialTracker, "native", "Tracks Fiducials");

#define MAX_FIDUCIALS 512

typedef struct {
	Segmenter segmenter;
	FiducialX fiducials[ MAX_FIDUCIALS ];
	RegionX regions[ MAX_FIDUCIALS * 4 ];
	TreeIdMap treeidmap;
	FidtrackerX fidtrackerx;
	ShortPoint *dmap;
} fiducials_data_t;

otFiducialTrackerModule::otFiducialTrackerModule() : otImageFilterModule() {
	MODULE_INIT();

	this->output_data = new otDataStream("otDataGenericList");
	this->output_count = 2;
	this->output_infos[1] = new otDataStreamInfo("data", "otDataGenericList", "Data stream with touch container");

	this->internal = malloc(sizeof(fiducials_data_t));
}

otFiducialTrackerModule::~otFiducialTrackerModule() {
}

void otFiducialTrackerModule::clearFiducials() {
	otDataGenericList::iterator it;
	for ( it = this->fiducials.begin(); it != this->fiducials.end(); it++ )
		delete (*it);
	this->fiducials.clear();
}


void otFiducialTrackerModule::allocateBuffers() {
	IplImage* src = (IplImage*)(this->input->getData());
	this->output_buffer = cvCreateImage(cvGetSize(src), src->depth, 3);	// only one channel
	LOG(DEBUG) << "allocated output buffer for FiducialTracker module.";

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

void otFiducialTrackerModule::applyFilter() {
	IplImage* src = (IplImage*)(this->input->getData());
	fiducials_data_t *fids = (fiducials_data_t *)this->internal;
	otDataGenericContainer *fiducial;
	FiducialX *fdx;
	int fid_count, valid_fiducials = 0;
	bool do_image = this->output->getObserverCount() > 0 ? true : false;

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 2);

	assert( src != NULL );
	assert( fids != NULL );
	assert( src->imageData != NULL );
	assert( "fiducial tracker needs single channel input" && (src->nChannels == 1) );

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

		LOGM(DEBUG) << "fid:" << i << " id=" << fdx->id << " pos=" \
			<< fdx->x << "," << fdx->y << " angle=" << fdx->angle;

		fiducial = new otDataGenericContainer();
		fiducial->properties["type"] = new otProperty("fiducial");
		fiducial->properties["id"] = new otProperty(fdx->id);
		fiducial->properties["x"] = new otProperty(fdx->x);
		fiducial->properties["y"] = new otProperty(fdx->y);
		fiducial->properties["angle"] = new otProperty(fdx->angle);
		fiducial->properties["leaf_size"] = new otProperty(fdx->leaf_size);
		fiducial->properties["root_size"] = new otProperty(fdx->root_size);
		this->fiducials.push_back(fiducial);

		// draw on output image
		if ( do_image ) {
			std::ostringstream oss;
			oss << fdx->id;
			cvPutText (this->output_buffer, oss.str().c_str(),
				cvPoint(fdx->x, fdx->y), &font, cvScalar(20, 255, 20));
		}
	}

	LOGM(DEBUG) << "-> Found " << valid_fiducials << " fiducials";
	this->output_data->push(&this->fiducials);
}

otDataStream* otFiducialTrackerModule::getOutput(int n) {
	if ( n == 1 )
		return this->output_data;
	return otImageFilterModule::getOutput(n);
}

