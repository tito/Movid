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
	this->output_names[1] = std::string("data");
	this->output_types[1] = std::string("otDataGenericList");

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
	otDataGenericContainer *fiducial = NULL;
	FiducialX *fdx = NULL;

	assert( src != NULL );
	assert( fids != NULL );
	assert( src->imageData != NULL );
	assert( "fiducial tracker needs single channel input" && (src->nChannels == 1) );

	step_segmenter(&fids->segmenter, (const unsigned char*)src->imageData);
	int fid_count = find_fiducialsX(fids->fiducials, MAX_FIDUCIALS,  &fids->fidtrackerx, &fids->segmenter, src->width, src->height);

	this->clearFiducials();

	int valid_fiducials = 0;;
	for ( int i = 0; i < fid_count; i++ ) {
		fdx = &fids->fiducials[i];

		// invalid id (INVALID_FIDUCIAL_ID)
		if ( fdx->id < 0 )
			continue;

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
	}

	LOGM(DEBUG) << "-> Found " << valid_fiducials << " fiducials";
}

otDataStream* otFiducialTrackerModule::getOutput(int n) {
	if ( n == 1 )
		return this->output_data;
	return otImageFilterModule::getOutput(n);
}

