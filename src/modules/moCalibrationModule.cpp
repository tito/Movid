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
//		 * Add support for moving/adding/deleting points
//		 * Add support for drawing transformed touches in the calibration grid


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
	if ( property->asBool() )
		module->resetCalibration();
}

void mocalibrationmodule_triangulate(moProperty *property, void *userdata)
{
	moCalibrationModule *module = static_cast<moCalibrationModule *>(userdata);
	assert(userdata != NULL);
	module->notifyTriangulate();
}

moCalibrationModule::moCalibrationModule() : moModule(MO_MODULE_INPUT | MO_MODULE_OUTPUT | MO_MODULE_GUI, 1, 1){

	MODULE_INIT();

	// declare input/output
	this->input_infos[0] = new moDataStreamInfo("data", "moDataGenericList", "Data stream with type of 'touch' or 'fiducial'");
	this->output_infos[0] = new moDataStreamInfo("data", "moDataGenericList", "Data stream with type of 'touch' or 'fiducial'");

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

	this->retriangulate = false;
	this->rect = cvRect(0, 0, 5000, 5000);
	this->storage = cvCreateMemStorage(0);
	this->active_point = 0;
	this->last_id = -1;
	this->input = NULL;
	this->output = NULL;
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

			if( org_pt && dst_pt )
			{
				org = this->delaunayToScreen[org_pt];
				dst = this->delaunayToScreen[dst_pt];

				//if ( abs(org.x) != 15000 && abs(org.y) != 15000 &&
				//	 abs(dst.x) != 15000 && abs(dst.y) != 15000)
				{
					oss.str("");
					oss << "line " << int(org.x * 1000.);
					oss << " " << int(org.y * 1000.);
					oss << " " << int(dst.x * 1000.);
					oss << " " << int(dst.y * 1000.);
					this->gui.push_back(oss.str());
				}
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
		for ( it = blobs.begin(); it != this->blobs.end(); it++ ) {
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

CvSubdiv2D* init_delaunay(CvMemStorage* storage, CvRect rect) {
    CvSubdiv2D* subdiv;
    subdiv = cvCreateSubdiv2D(CV_SEQ_KIND_SUBDIV2D,
							  sizeof(*subdiv),
							  sizeof(CvSubdiv2DPoint),
							  sizeof(CvQuadEdge2D),
							  storage);
    cvInitSubdivDelaunay2D(subdiv, rect);
    return subdiv;
}

void moCalibrationModule::triangulate() {
	// We first triangulate all the surfacePoints.
	// Afterwards, in transform mode when a new touch occurrs, we can
	// simply look up the triangle in which the touch was performed
	// and get the barycentric parameters of the touch in that triangle.
	// We then use these to compute the on screen coordinate of the touch.
	moPointList screenPoints = this->property("screenPoints").asPointList(),
				surfacePoints = this->property("surfacePoints").asPointList();
	assert(screenPoints.size() == surfacePoints.size());

	moPointList::iterator it, its;
	//std::vector<moPoint>::iterator it;
	this->delaunayToScreen.clear();
	this->subdiv = init_delaunay(this->storage, this->rect);
	for(it = surfacePoints.begin(),
		its = screenPoints.begin();
		it != surfacePoints.end();
		it++, its++) {
		CvPoint2D32f fp = cvPoint2D32f(it->x, it->y);
		CvSubdiv2DPoint *delaunayPoint = cvSubdivDelaunay2DInsert(subdiv, fp);
		std::cout << delaunayPoint << ":" << delaunayPoint->pt.x << "," << delaunayPoint->pt.y << std::endl;
		std::cout << "+> " << its->x << "," << its->y << std::endl;
		this->delaunayToScreen[delaunayPoint] = (*its);
	}
	//cvCalcSubdivVoronoi2D(this->subdiv);
	this->retriangulate = false;
	this->notifyGui();
}

void moCalibrationModule::calibrate() {
	moDataGenericList *blobs = static_cast<moDataGenericList*>(input->getData());
	moDataGenericContainer *touch;
	moPointList screenPoints;
	moPoint surfacePoint, p;

	LOG(MO_DEBUG) << "#Blobs in frame: " << blobs->size();

	// We only calibrate the current point if there is an unambiguous amount of touches
	if (blobs->size() != 1)
		return;

	// We now want to assign each point its coordinates on the touch surface
	touch = (*blobs)[0];

	// Don't reuse the same touch id as before
	if ( touch->properties["id"]->asInteger() == (int)this->last_id )
		return;
	this->last_id = touch->properties["id"]->asInteger();

	// TODO While calibrating, this list MUST NOT change!
	screenPoints = this->property("screenPoints").asPointList();
	if (this->active_point == screenPoints.size()) {
		// We have calibrated all points, so we're done.
		LOG(MO_DEBUG) << "Calibration complete!";
		this->active_point = 0;
		this->property("calibrate").set(false);
		if (this->retriangulate)
			this->triangulate();
		return;
	}

	LOG(MO_DEBUG) << "# of screenPoints: " << screenPoints.size();
	LOG(MO_DEBUG) << "Processing point #" << this->active_point;

	// We're starting calibration again, so discard all old calibration results
	if ( this->active_point == 0 )
		this->property("surfacePoints").set("");

	// screenPoints and corresponding surfacePoint have the same index in their respective vector
	moPointList surfacePoints = this->property("surfacePoints").asPointList();
	surfacePoint.x = touch->properties["x"]->asDouble();
	surfacePoint.y = touch->properties["y"]->asDouble();
	surfacePoints.push_back(surfacePoint);
	this->property("surfacePoints").set(surfacePoints);
	
	p = screenPoints[this->active_point];
	LOG(MO_DEBUG) << "(" << p.x << ", " << p.y << ") is mapped to (" \
		<< surfacePoint.x << ", " << surfacePoint.y << ").";

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
		CvSubdiv2DEdge e;
		CvSubdiv2DEdge e0 = NULL;
		//CvSubdiv2DEdge edges[3];
		//CvSubdiv2DPoint* vertices[3];
		CvSubdiv2DPoint* v;
		CvSubdiv2DPoint* p = NULL;
		double touch_x = (*it)->properties["x"]->asDouble();
		double touch_y = (*it)->properties["y"]->asDouble();
		LOG(MO_DEBUG) << std::endl << "Incoming touch is at " << touch_x << ", " << touch_y;
		CvPoint2D32f fp = cvPoint2D32f(touch_x, touch_y);
		cvSubdiv2DLocate(this->subdiv, fp, &e0, &p);
		CvSubdiv2DPoint* enclosingPoints[3];
		
		if (!e0) {
			assert(0);
			return; // XXX why is this necessary?
		}
		
		//edges[0] = e0;
		// Collect the vertices of the triangle enclosing the given surfacePoint
		e = e0;
		int i = 0;
		v = cvSubdiv2DEdgeOrg(e0);
		do {
			e = cvSubdiv2DGetEdge(e, CV_NEXT_AROUND_LEFT);
			//edges[i++] = e;
			//vertices[i++] = cvSubdiv2DEdgeOrg(e);
			enclosingPoints[i++] = v;
			v = cvSubdiv2DEdgeOrg(e);			
		}
		while (e != e0);
		for (i = 0; i < 3; i++) {
			LOG(MO_DEBUG) << "Found point " << i << " for touch " << 
			(*it)->properties["id"]->asInteger() << ": " << (*enclosingPoints[i]).pt.x << "/" << (*enclosingPoints[i]).pt.y;
		}
		// Now that we found the three surfacePoints of the triangle enclosing the given point,
		// we can do the barycentric conversion.
		CvPoint2D32f surf_a, surf_b, surf_c, p1, p2;
		moPoint screen_a, screen_b, screen_c;
		double alpha, beta, gamma;
		surf_a = enclosingPoints[0]->pt;
		screen_a = this->delaunayToScreen[enclosingPoints[0]];
		// We want to use the point with the smallest y-difference as point b
		p1 = enclosingPoints[1]->pt;
		p2 = enclosingPoints[2]->pt;
		// XXX > ????
		if (abs(p1.y - surf_a.y) > abs(p2.y - surf_a.y)) {
			surf_b = p1;
			screen_b = this->delaunayToScreen[enclosingPoints[1]];
			surf_c = p2;
			screen_c = this->delaunayToScreen[enclosingPoints[2]];
		}
		else {
			surf_b = p2;
			screen_b = this->delaunayToScreen[enclosingPoints[2]];
			surf_c = p1;
			screen_c = this->delaunayToScreen[enclosingPoints[1]];
		}
		LOG(MO_DEBUG) << "[Surf_A: " << surf_a.x << " / " << surf_a.y << "] [Surf_B: " << surf_b.x << " / " << surf_b.y << "] [Surf_C: " << surf_c.x << " / " << surf_c.y << "]";
		// TODO Should be vectors...
		moPoint ab = {surf_b.x - surf_a.x, surf_b.y - surf_a.y};
		moPoint ac = {surf_c.x - surf_a.x, surf_c.y - surf_a.y};
		LOG(MO_DEBUG) << "[Vector ab: " << ab.x << " / " << ab.y << "] [Vector ac " << ac.x << " / " << ac.y << "]";
		beta = (touch_x - surf_a.x) / ab.x;
		LOG(MO_DEBUG) << "Beta = |" << touch_x << " - " << surf_a.x << "| / " << ab.x << " = " << beta;
		gamma = (touch_y - surf_a.y) / ac.y;
		LOG(MO_DEBUG) << "Gamma = |" << touch_y << " - " << surf_a.y << "| / " << ac.y << " = " << gamma;
		alpha = (1.0 - beta - gamma);

		double final_x = screen_a.x * alpha + screen_b.x * beta + screen_c.x * gamma;
		double final_y = screen_a.y * alpha + screen_b.y * beta + screen_c.y * gamma;

		LOG(MO_DEBUG) << "Barycentric params for touch " << (*it)->properties["id"]->asInteger()
					  << ": " << alpha << "/" << beta << "/" << gamma << " += " << alpha+beta+gamma;
		LOG(MO_DEBUG) << "Final Touch is at: " << final_x << "/" << final_y;
		
		// add the blob in data
		moDataGenericContainer *touch = new moDataGenericContainer();
		touch->properties["type"] = new moProperty("touch");
		touch->properties["id"] = new moProperty((*it)->properties["id"]->asInteger());
		touch->properties["x"] = new moProperty(final_x);
		touch->properties["y"] = new moProperty(final_y);
		// double correct?
		touch->properties["w"] = new moProperty((*it)->properties["w"]->asDouble());
		touch->properties["h"] = new moProperty((*it)->properties["h"]->asDouble());
		this->blobs.push_back(touch);
	}

	if (this->output != NULL) delete this->output;
	this->output = new moDataStream("GenericTouch");
	this->output->push(&this->blobs);

	// new blobs... gui must be updated
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
		// Perhaps points were added, moved or deleted. If this is the case
		// we have to triangulate again. 
	} else {
		if (this->retriangulate)
			this->triangulate();
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

void moCalibrationModule::setInput(moDataStream *stream, int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return;
	}
	if ( this->input != NULL )
		this->input->removeObserver(this);
	this->input = stream;
	if ( stream != NULL ) {
		if ( stream->getFormat() != "GenericTouch" &&
			stream->getFormat() != "GenericFiducial" ) {
			this->setError("Input 0 accept only touch or fiducial");
			this->input = NULL;
			return;
		}
	}
	if ( this->input != NULL )
		this->input->addObserver(this);
}

moDataStream* moCalibrationModule::getInput(int n) {
	if ( n != 0 ) {
		this->setError("Invalid input index");
		return NULL;
	}
	return this->input;
}

moDataStream* moCalibrationModule::getOutput(int n) {
	return this->output;
}
