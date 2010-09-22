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


// TODO:
//		 * Cleanup
//		 * Comments
//		 * Tests & Fixes
//		 * properly delete/release all objects in the destructor
//		 * move variable declarations to the beginning of each function
//
//	Tito:
//		 * Add support for moving/adding/deleting points
//		 * Add SDL (e.g.) fullscreen window. Calib in browser sucks
//		 * Make screenPoints list immutable while calibrating


#include <assert.h>
#include "sstream"
#include "moCalibrationModule.h"
#include "../moLog.h"
#include <stdio.h>

// XXX Desc
MODULE_DECLARE(Calibration, "native", "Calibration");

// callback used for rows and cols. if these properties is updated,
// refresh the GUI.
void mocalibrationmodule_update_size(moProperty *property, void *userdata)
{
	moCalibrationModule *module = static_cast<moCalibrationModule *>(userdata);
	assert(userdata != NULL);
	module->buildScreenPoints();
}

void mocalibrationmodule_activate_calibration(moProperty *property, void *userdata)
{
	moCalibrationModule *module = static_cast<moCalibrationModule *>(userdata);
	assert(userdata != NULL);
	LOG(MO_DEBUG, "Setting calibration prperty" << property->asBool() << "was: " << module->property("calibrate").asBool());
	if ( property->asBool() )
		module->resetCalibration();
}

void mocalibrationmodule_triangulate(moProperty *property, void *userdata)
{
	moCalibrationModule *module = static_cast<moCalibrationModule *>(userdata);
	assert(userdata != NULL);
	LOG(MO_DEBUG, "Setting triagulate prperty" << property->asBool() << "was: " << module->property("retriangulate").asBool());
	module->notifyTriangulate();
}

moCalibrationModule::moCalibrationModule() : moModule(MO_MODULE_INPUT | MO_MODULE_OUTPUT | MO_MODULE_GUI){

	MODULE_INIT();

	// declare input/output

	this->input = NULL;
	this->output = new moDataStream("trackedblob");
	this->declareInput(0, &this->input, new moDataStreamInfo(
			"data", "trackedblob", "Data stream with type of 'trackedblob'"));
	this->declareOutput(0, &this->output, new moDataStreamInfo(
			"data", "trackedblob", "Data stream with type of 'trackedblob'"));

	this->properties["rows"] = new moProperty(3);
	this->properties["rows"]->setMin(2);
	this->properties["rows"]->addCallback(mocalibrationmodule_update_size, this);
	this->properties["cols"] = new moProperty(3);
	this->properties["cols"]->setMin(2);
	this->properties["cols"]->addCallback(mocalibrationmodule_update_size, this);
	this->properties["screenPoints"] = new moProperty(moPointList());
	this->properties["surfacePoints"] = new moProperty(moPointList());
	this->properties["surfacePoints"]->addCallback(mocalibrationmodule_triangulate, this);
	this->properties["calibrate"] = new moProperty(false);
	this->properties["calibrate"]->addCallback(mocalibrationmodule_activate_calibration, this);
	// Minimum frames that the user has to press a calibration point
	this->properties["duration_per_point"] = new moProperty(50);
	this->properties["duration_per_point"]->addCallback(mocalibrationmodule_activate_calibration, this);

	this->retriangulate = false;
	this->rect = cvRect(0, 0, 5000, 5000);
	this->storage = cvCreateMemStorage(0);
	this->active_point = 0;
	this->last_id = -1;
	this->last_finished_id = -1;
	this->current_duration = 0;
	this->current_touch = NULL;
	this->calibrated = false;

	this->subdiv = NULL;

	this->buildScreenPoints();
}

void moCalibrationModule::resetCalibration() {
	this->last_id = -1;
	this->active_point = 0;
}

void moCalibrationModule::buildScreenPoints() {
	// TODO handle saving of additionnal point in another list than screenPoints
	int mx = this->property("cols").asInteger();
	int my = this->property("rows").asInteger();
	float dx = 1.0 / ((float)mx - 1);
	float dy = 1.0 / ((float)my - 1);
	std::ostringstream oss;
	oss.str("");
	for ( int x = 0; x < mx; x++ ) {
		for ( int y = 0; y < my; y++ ) {
			oss << (x * dx) << ",";
			oss << (y * dy) << ";";
		}
	}

	this->property("screenPoints").set(oss.str());
	this->notifyGui();
}

moCalibrationModule::~moCalibrationModule() {
	cvReleaseMemStorage(&(this->storage));
}

void moCalibrationModule::guiFeedback(const std::string& type, double x, double y) {
	this->notifyGui();
}

void moCalibrationModule::guiBuild(void) {
	std::ostringstream oss;
	moPointList screenPoints = this->property("screenPoints").asPointList();
	moPointList::iterator it;
	unsigned int index = 0;

	this->gui.clear();
	this->gui.push_back("viewport 1000 1000");
	this->gui.push_back("color 0 121 184");

	for ( it = screenPoints.begin(); it != screenPoints.end(); it++ ) {
		if ( index == this->active_point )
			this->gui.push_back("color 255 255 255");

		oss.str("");
		oss << "circle " << (int)(it->x * 1000.) << " " << (int)(it->y * 1000.) << " 50";
		this->gui.push_back(oss.str());

		if ( index == this->active_point )
			this->gui.push_back("color 120 120 120");

		index++;
	}

	// draw delaunay triangles ?
	if ( this->subdiv == NULL )
		return;

	this->gui.push_back("color 255 255 255");

    CvSeqReader  reader;
    int i, total = this->subdiv->edges->total;
    int elem_size = this->subdiv->edges->elem_size;

    cvStartReadSeq( (CvSeq*)(this->subdiv->edges), &reader, 0 );

    for( i = 0; i < total; i++ )
    {
        CvQuadEdge2D* edge = (CvQuadEdge2D*)(reader.ptr);

        if( CV_IS_SET_ELEM( edge ))
        {
			CvSubdiv2DPoint* org_pt;
			CvSubdiv2DPoint* dst_pt;
			moPoint org;
			moPoint dst;

			org_pt = cvSubdiv2DEdgeOrg((CvSubdiv2DEdge)edge);
			dst_pt = cvSubdiv2DEdgeDst((CvSubdiv2DEdge)edge);

           if(  org_pt && dst_pt &&
                 this->delaunayToScreen.find(org_pt) != this->delaunayToScreen.end() &&
                 this->delaunayToScreen.find(dst_pt) != this->delaunayToScreen.end() )

			{
				org = this->delaunayToScreen[org_pt];
				dst = this->delaunayToScreen[dst_pt];

				oss.str("");
				oss << "line " << int(org.x * 1000.);
				oss << " " << int(org.y * 1000.);
				oss << " " << int(dst.x * 1000.);
				oss << " " << int(dst.y * 1000.);
				this->gui.push_back(oss.str());
				LOG(MO_DEBUG, "drawing line: " << i << "   surface pos:" << org_pt->pt.x << ","<< org_pt->pt.y<<"|"<<dst_pt->pt.x << ","<< dst_pt->pt.y);
				LOG(MO_DEBUG, "    creen points: " << oss.str());
			}
        }

        CV_NEXT_SEQ_ELEM( elem_size, reader );
    }

	// draw touches (since touch can be changed, if we lock input
	// we'll not hve trouble)
	if ( this->input != NULL ) {
		moDataGenericList::iterator it;
		int x, y;
		this->input->lock();
		for ( it = this->blobs.begin(); it != this->blobs.end(); it++ ) {
			x = (int)((*it)->properties["x"]->asDouble() * 1000.);
			y = (int)((*it)->properties["y"]->asDouble() * 1000.);

			this->gui.push_back("color 121 0 184");

			oss.str("");
			oss << "circle " << x << " " << y << " 80";
			this->gui.push_back(oss.str());

			this->gui.push_back("color 255 255 255");

			oss.str("");
			oss << "line " << x - 160 << " " << y << " " << x + 160 << " " << y;
			this->gui.push_back(oss.str());

			oss.str("");
			oss << "line " << x << " " << y - 160 << " " << x << " " << y + 160;
			this->gui.push_back(oss.str());
		}
		this->input->unlock();
	}
}

void moCalibrationModule::triangulate() {
	// We first triangulate all the surfacePoints.
	// Afterwards, in transform mode when a new touch occurrs, we can
	// simply look up the triangle in which the touch was performed
	// and get the barycentric parameters of the touch in that triangle.
	// We then use these to compute the on screen coordinate of the touch.
	moPointList screenPoints  = this->property("screenPoints").asPointList();
	moPointList surfacePoints = this->property("surfacePoints").asPointList();
	assert(screenPoints.size() == surfacePoints.size());

	this->delaunayToScreen.clear();
    this->subdiv = cvCreateSubdivDelaunay2D(this->rect, this->storage);

	//add all the  surfacepoints we collected to the subdivision
	//use the delaunayToScreen map to associate them with corrosponding screen point
	moPointList::iterator it, its;
	for(it = surfacePoints.begin(), its = screenPoints.begin(); it != surfacePoints.end();  it++, its++) {
		CvPoint2D32f fp = cvPoint2D32f(it->x, it->y);
		CvSubdiv2DPoint *delaunayPoint = cvSubdivDelaunay2DInsert(subdiv, fp);
		this->delaunayToScreen[delaunayPoint] = (*its);
	}

	this->retriangulate = false;
	this->notifyGui();
}

void moCalibrationModule::calibrate() {
	moDataGenericList *blobs = static_cast<moDataGenericList*>(input->getData());
	moDataGenericContainer *touch;
	moPointList screenPoints;
	moPoint surfacePoint, p;

	// We only calibrate the current point if there is an unambiguous amount of touches
	if (blobs->size() != 1)
		return;

	// We now want to assign each point its coordinates on the touch surface
	touch = (*blobs)[0];

	// We're starting calibration again, so discard all old calibration results
	if ( this->active_point == 0 )
		this->property("surfacePoints").set("");

	// screenPoints and corresponding surfacePoint have the same index in their respective vector
	moPointList surfacePoints = this->property("surfacePoints").asPointList();
	surfacePoint.x = touch->properties["x"]->asDouble();
	surfacePoint.y = touch->properties["y"]->asDouble();

	unsigned int blob_id = touch->properties["blob_id"]->asInteger();
	if (blob_id == this->last_id) {
		// This means we have seen this touch before in the previous frame.
		// TODO refine
		// Don't reuse it to calibrate the next point, but refine the surfacePoints
		// position by taking the mean. Useful for e.g. laser tracking where you don't
		// hit the calibration point right immediately.
		//unsigned int last_index = surfacePoints.size()-1;
		if (this->last_finished_id == blob_id) {
			return;
		}
		if (this->current_touch == NULL) {
			this->current_touch = &(surfacePoint);
			assert(this->current_duration == 0);
		}
		this->current_touch->x = (this->current_touch->x + surfacePoint.x) / 2.;
		this->current_touch->y = (this->current_touch->y + surfacePoint.y) / 2.;
		this->current_duration++;
	}
	else {
		// The calibration touch was interrupted. Start new calibration attempt for that touch.
		this->current_touch = NULL;
		this->current_duration = 0;
		this->last_id = -1;
	}

	this->last_id = touch->properties["blob_id"]->asInteger();

	if (this->current_duration < this->property("duration_per_point").asInteger()) {
		return;
	}

	// OK, this point was calibrated long enough
	// Indicate that this blob id is finished and shouldn't be used to calibrate anymore
	this->last_finished_id = this->last_id;
	assert(last_id == blob_id);
	surfacePoints.push_back(*(this->current_touch));
	this->current_touch = NULL;
	this->current_duration = 0;
	this->property("surfacePoints").set(surfacePoints);

	LOG(MO_DEBUG, "calibrarting   # of screenPoints: " << screenPoints.size() << "# of surfacePoints: " << surfacePoints.size());
	LOG(MO_DEBUG, "Processing point #" << this->active_point);

	// TODO While calibrating, this list MUST NOT change!
	screenPoints = this->property("screenPoints").asPointList();
	if (this->active_point == screenPoints.size()-1) {
		// We have calibrated all points, so we're done.
		LOG(MO_DEBUG, "Calibration complete!");
		this->active_point = 0;
		this->property("calibrate").set(false);
		this->calibrated = true;
		//if (this->retriangulate)
			this->triangulate();
		return;
	}

	LOG(MO_DEBUG, "# of screenPoints: " << screenPoints.size());
	LOG(MO_DEBUG, "Processing point #" << this->active_point);

	//surfacePoints.push_back(surfacePoint);
	//this->property("surfacePoints").set(surfacePoints);

	p = screenPoints[this->active_point];
	LOG(MO_DEBUG, "(" << p.x << ", " << p.y << ") is mapped to (" \
		<< surfacePoint.x << ", " << surfacePoint.y << ").");

	// Proceed with the next grid point
	this->active_point++;

	// Inform gui to update
	this->notifyGui();
}

void moCalibrationModule::transformPoints() {
	// Calibration & triangulation is done. Just convert the point coordinates.
	moDataGenericList *blobs = static_cast<moDataGenericList*>(input->getData());
	moDataGenericList::iterator it;

	this->blobs.clear();
	for (it = blobs->begin(); it != blobs->end(); it++) {
		// Get the camera/surface coordinates of the blob
		double blob_x = (*it)->properties["x"]->asDouble();
		double blob_y = (*it)->properties["y"]->asDouble();

		// Find the closest edge the point falls onto or right to
		CvSubdiv2DEdge edge;
		CvSubdiv2DPoint* vertex;
		CvPoint2D32f P = cvPoint2D32f(blob_x, blob_y);
		CvSubdiv2DPointLocation loc;
		loc = cvSubdiv2DLocate(this->subdiv, P, &edge, &vertex);
		if (loc == CV_PTLOC_VERTEX) {
			moPoint screen_point = this->delaunayToScreen[vertex];
			moDataGenericContainer *blob = new moDataGenericContainer();
			blob->properties["type"] = new moProperty("trackedblob");
			blob->properties["blob_id"] = new moProperty((*it)->properties["blob_id"]->asInteger());
			blob->properties["implements"] = new moProperty((*it)->properties["implements"]->asString());
			// FIXME copy all the properties, not just x/y
			blob->properties["x"] = new moProperty(screen_point.x);
			blob->properties["y"] = new moProperty(screen_point.y);
			this->blobs.push_back(blob);
			continue;
		}
		// P is inside the triangle, so we must compute barycentric coords for P with
		// respect to the triangle. To find the triangle, we traverse the edges
		// around the right facet, to get the vertices that make up the triangle containing P.
		//
		// A, B, C are given in surface coordinates!
		CvSubdiv2DPoint* A = cvSubdiv2DEdgeOrg(edge);
		edge = cvSubdiv2DGetEdge( edge, CV_NEXT_AROUND_RIGHT );
		CvSubdiv2DPoint* B = cvSubdiv2DEdgeOrg(edge);
		edge = cvSubdiv2DGetEdge( edge, CV_NEXT_AROUND_RIGHT );
		CvSubdiv2DPoint* C = cvSubdiv2DEdgeOrg(edge);
		CvPoint2D32f a = A->pt,
					 b = B->pt,
					 c = C->pt;

		// XXX Should this computation turn out to be too costly for each blob in each frame,
		//	   it can be speeded up by computing a map of surface2screen points for each pixel
		//	   of the camera image and then just looking the appropriate coordinates up from the map!
		//
		// This is one way to compute the barycentric coordinates. Other attempts use
		// matrices and determinants or stuff like that. This approach suffices here.
		float entire_area = (a.x - b.x) * (a.y - c.y) - (a.y - b.y) * (a.x - c.x);
		float area_a = (blob_x - b.x) * (blob_y - c.y) - (blob_y - b.y) * (blob_x - c.x);
		float area_b = (a.x - blob_x) * (a.y - c.y) - (a.y - blob_y) * (a.x - c.x);

		// Compute the barycentric coords alpha, beta, gamma based on the area.
		float alpha = area_a / entire_area;
		float beta = area_b / entire_area;
		// Set alpha + beta + gamma == 1 (True if point inside triangle,
		// with alpha, beta and gamma each between 0 and 1)
		float gamma = 1.0f - alpha - beta;

		moPoint A_screen = this->delaunayToScreen[A];
		moPoint B_screen = this->delaunayToScreen[B];
		moPoint C_screen = this->delaunayToScreen[C];

		// Transform the point into screen space by interpolating the three vertices
		// of the enclosing triangle in screen space with the barycentric coordinates.
		moPoint P_transformed;
		P_transformed.x = alpha*A_screen.x + beta*B_screen.x + gamma*C_screen.x;
		P_transformed.y = alpha*A_screen.y + beta*B_screen.y + gamma*C_screen.y;

		// This must copy ALL blob attributes, even unknown ones (from different detectors)
		// Only adjust type, pos.
		// XXX Do we need to adjust w/h too?
		moDataGenericContainer *blob = (*it)->clone();
		blob->properties["type"] = new moProperty("trackedblob");
		blob->properties["x"] = new moProperty(P_transformed.x);
		blob->properties["y"] = new moProperty(P_transformed.y);
		this->blobs.push_back(blob);

		LOG(MO_DEBUG, "transformed Point |" << " in: " << P.x << "," << P.y
					  << " out: " << P_transformed.x << "," << P_transformed.y);
	}

	this->output->push(&this->blobs);
	this->notifyGui();
}

void moCalibrationModule::notifyTriangulate() {
	this->retriangulate = true;
}

void moCalibrationModule::update() {
	bool calibrate = this->property("calibrate").asBool();

	assert(this->input != NULL);

	this->input->lock();

	if ( calibrate ) {
		this->calibrate();
	} else {
		if (this->calibrated)
			this->transformPoints();
	}
	this->input->unlock();
}

void moCalibrationModule::notifyData(moDataStream *input) {
	assert( input != NULL );
	assert( input == this->input );
	this->notifyUpdate();
}

void moCalibrationModule::start() {
	moModule::start();
}

void moCalibrationModule::stop() {
	moModule::stop();
}
