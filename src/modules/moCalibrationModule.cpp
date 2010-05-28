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
	LOG(MO_DEBUG) << "Setting calibration prperty" << property->asBool() << "was: " << module->property("calibrate").asBool() ;
	if ( property->asBool() )
		module->resetCalibration();
}

void mocalibrationmodule_triangulate(moProperty *property, void *userdata)
{
	moCalibrationModule *module = static_cast<moCalibrationModule *>(userdata);
	assert(userdata != NULL);
	LOG(MO_DEBUG) << "Setting triagulate prperty" << property->asBool() << "was: " << module->property("retriangulate").asBool() ;
	module->notifyTriangulate();
}

moCalibrationModule::moCalibrationModule() : moModule(MO_MODULE_INPUT | MO_MODULE_OUTPUT | MO_MODULE_GUI, 1, 1){

	MODULE_INIT();

	// declare input/output
	
	this->output = new moDataStream("GenericTouch");
	this->input_infos[0] = new moDataStreamInfo("data", "moDataGenericList", "Data stream with type of 'touch' or 'fiducial'");
	this->output_infos[0] = new moDataStreamInfo("data", "GenericTouch", "Data stream with type of 'touch' or 'fiducial'");

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
				LOG(MO_DEBUG) << "drawing line: " << i << "   surface pos:" << org_pt->pt.x << ","<< org_pt->pt.y<<"|"<<dst_pt->pt.x << ","<< dst_pt->pt.y;
				LOG(MO_DEBUG) << "    creen points: " << oss.str();
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

	//LOG(MO_DEBUG) << "#Blobs in frame: " << blobs->size();

	// We only calibrate the current point if there is an unambiguous amount of touches
	if (blobs->size() != 1)
		return;

	// We now want to assign each point its coordinates on the touch surface
	touch = (*blobs)[0];

	// Don't reuse the same touch id as before
	if ( touch->properties["id"]->asInteger() == (int)this->last_id )
		return;
	this->last_id = touch->properties["id"]->asInteger();

	LOG(MO_DEBUG) << "calibrarting   # of screenPoints: " << screenPoints.size() << "# of surfacePoints: " << surfacePoints.size() ;
	LOG(MO_DEBUG) << "Processing point #" << this->active_point;

	// TODO While calibrating, this list MUST NOT change!
	screenPoints = this->property("screenPoints").asPointList();
	if (this->active_point == screenPoints.size()) {
		// We have calibrated all points, so we're done.
		LOG(MO_DEBUG) << "Calibration complete!";
		this->active_point = 0;
		this->property("calibrate").set(false);
		this->calibrated = true;
		//if (this->retriangulate)
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
	
	bool pushed = false;
	this->blobs.clear();
	for (it = blobs->begin(); it != blobs->end(); it++) {
		
		//create a new data container copying blob properties, 
		//well set x, and y at the end when weve calculated them
		moDataGenericContainer *touch = new moDataGenericContainer();
		touch->properties["type"] = new moProperty("touch");
		touch->properties["id"] = new moProperty((*it)->properties["id"]->asInteger());
		touch->properties["w"] = new moProperty((*it)->properties["w"]->asDouble());
		touch->properties["h"] = new moProperty((*it)->properties["h"]->asDouble());
		
				
		//P is the surafce coordinates of the blob were trying to transform to screen coordinates
		double blob_x = (*it)->properties["x"]->asDouble();
		double blob_y = (*it)->properties["y"]->asDouble();
		CvPoint2D32f P = cvPoint2D32f(blob_x, blob_y);

		//the transformed point P in screen space, this is what we are going to calculate
		moPoint P_transformed; 


		//find the edge closest to the point given using cvSubdiv2DLocate
		//see http://opencv.willowgarage.com/documentation/planar_subdivisions.html#subdiv2dlocate
		//   the CvSubdiv2DPointLocation return value determines what type of location was found
		//   if P falls on an edge or inside a triangle/facade of teh subdivsion:
		//      edge will be the output edge the point falls onto or right to
		//   if P falls directly onto a vertex of teh subdivision:
		//      vertex will be the vertex of the subdivision with which P coincides directly
		CvSubdiv2DEdge edge;
		CvSubdiv2DPoint* vertex;
		CvSubdiv2DPointLocation location = cvSubdiv2DLocate(this->subdiv, P, &edge, &vertex);
		
//		if (location == CV_PTLOC_VERTEX){
//			//P coincides directly with a vertex from the delaunay subdivision, so the value is ready to go
//			P_transformed = this->delaunayToScreen[vertex]; 
//		}	
//		else{
//		    P_transformed = this->delaunayToScreen[cvSubdiv2DEdgeOrg(edge)]; 
//		}
//		
//		
//		if (location == CV_PTLOC_ON_EDGE){
//			//the point falls directly onto an edge, so all we have to do is a linear interpolation
//		    CvSubdiv2DPoint* A_surf = cvSubdiv2DEdgeOrg(edge);
//		    CvSubdiv2DPoint* B_surf = cvSubdiv2DEdgeDst(edge);
//		    
//		    //compute ratio of distances between AB and AP to see where on the line teh vertex lies
//			CvPoint2D32f A = A_surf->pt;
//		    CvPoint2D32f B = B_surf->pt;
//		    double distAB = (B.x-A.x)*(B.x-A.x) + (B.y-A.y)*(B.y-A.y);
//		    double distAP = (P.x-A.x)*(P.x-A.x) + (P.y-A.y)*(P.y-A.y);
//		    double ratio = distAP/distAB;
//		    
//			moPoint A_screen = this->delaunayToScreen[A_surf];
//		    moPoint B_screen = this->delaunayToScreen[B_surf];
//		    
//		    P_transformed.x =  ratio*(B_screen.x-A_screen.x) + A_screen.x;
//		    P_transformed.y =  ratio*(B_screen.y-A_screen.y) + A_screen.y;
//		}
		if (location == CV_PTLOC_INSIDE){
			//P is inside the trinagle, so we must compute baycentric coordinates
			//we traverse the edges around the right facet, to get the vertices 
		    //that make up the triangle that contains P
			//
			// A, B, C are given in surface coordinates!
		    CvSubdiv2DPoint* A = cvSubdiv2DEdgeOrg(edge);
		    edge = cvSubdiv2DGetEdge( edge, CV_NEXT_AROUND_RIGHT );
		    CvSubdiv2DPoint* B = cvSubdiv2DEdgeOrg(edge);
		    edge = cvSubdiv2DGetEdge( edge, CV_NEXT_AROUND_RIGHT );
		    CvSubdiv2DPoint* C = cvSubdiv2DEdgeOrg(edge);
			CvPoint2D32f asurf = A->pt,
						 bsurf = B->pt,
						 csurf = C->pt;

			float entire_area = (asurf.x - bsurf.x) * (asurf.y - csurf.y) - (asurf.y - bsurf.y) * (asurf.x - csurf.x);
			float area_A = (blob_x - bsurf.x) * (blob_y - csurf.y) - (blob_y - bsurf.y) * (blob_x - csurf.x);
			float area_B = (asurf.x - blob_x) * (asurf.y - csurf.y) - (asurf.y - blob_y) * (asurf.x - csurf.x);

			float alpha = area_A / entire_area;
			float beta = area_B / entire_area;
			float gamma = 1.0f - alpha - beta;
		    
	    
		    moPoint A_screen = this->delaunayToScreen[A];
		    moPoint B_screen = this->delaunayToScreen[B];
		    moPoint C_screen = this->delaunayToScreen[C];
		    
		    P_transformed.x = alpha*A_screen.x + beta*B_screen.x + gamma*C_screen.x;
		    P_transformed.y = alpha*A_screen.y + beta*B_screen.y + gamma*C_screen.y;
		}
		
		
		//all done P_transformed shuld be calculated correctly now
		touch->properties["x"] = new moProperty(P_transformed.x);
		touch->properties["y"] = new moProperty(P_transformed.y);
		this->blobs.push_back(touch);
		pushed = true;
		
		LOG(MO_DEBUG) << "transformed Point |" << " in: " << P.x << "," << P.y 
					  << " out: " << P_transformed.x << "," << P_transformed.y;
	}

	if (pushed) {
		if (this->output != NULL) delete this->output;
		this->output = new moDataStream("GenericTouch");
		this->output->push(&this->blobs);
		this->notifyGui();
	}
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
		//if (this->retriangulate)
		//	this->triangulate();
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
