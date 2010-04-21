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
#include "moCalibrationModule.h"
#include "../moLog.h"
#include "cv.h"
#include <stdio.h>

// XXX Desc
MODULE_DECLARE(Calibration, "native", "Calibration");

//XXX This shouldn't be an imagefiltermodule
moCalibrationModule::moCalibrationModule() : moImageFilterModule(){

	MODULE_INIT();

	this->properties["rows"] = new moProperty(3);
	this->properties["cols"] = new moProperty(3);
	this->properties["screenPoints"] = new moProperty(moPointList());
	this->properties["do_calibration"] = new moProperty(true);
	this->properties["needs_retriangulation"] = new moProperty(true);
	// XXX
	this->rect = cvRect(0, 0, 5000, 5000);
	this->storage = cvCreateMemStorage(0);
	this->active_point = 0;
}

moCalibrationModule::~moCalibrationModule() {
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

void moCalibrationModule::triangulate() {/*
	std::cout << "Points:" << std::endl;
	moPointList points = this->property("grid_points").asPointList();
	moPointList::iterator it;
	this->subdiv = init_delaunay(this->storage, this->rect);
	for(it = points.begin(); it != points.end() ; it++) {
		std::cout << (*it).x << " " << (*it).y << std::endl;
		// XXX crashes here. cvPoint2D32f is a convenience macro for double to cvp2d32f conversion
		CvPoint2D32f fp = cvPoint2D32f((*it).x, (*it).y);
		//{static_cast<float>((*it).x),
		//static_cast<float>((*it).y)};
		cvSubdivDelaunay2DInsert(subdiv, fp);
	}
	// XXX Is triangulation performed now already?

	// Take the result of the triangulation and put them into our triangle struct
	// TODO
	this->property("needs_retriangulation") = false;
	*/
}

void moCalibrationModule::applyFilter() {
	bool calibrate = this->property("do_calibration").asBool();
	bool triangulate = this->property("needs_retriangulation").asBool();
	if (calibrate) {
		moDataGenericList *blobs = static_cast<moDataGenericList*>(this->input->getData());
		//std::cout << blobs->size() << std::endl;
		if (blobs->size() != 1) return;
		moDataGenericContainer *touch = (*blobs)[0];
		std::cout << touch->properties["type"]->asString() << std::endl;
		//for (it = blobs->begin(); it != blobs->end(); it++) {
		//	std::cout << (*it)->properties["type"]->asString() << std::endl;
		//}
		// We now assign each point its coordinates on the touch surface
/*		moPointList* points = this->property("grid_points").asPointList();
		moPointList* mapping = this->mapping;
		mapping.erase();
		// We ignore calibration attempts where not exactly one touch is present

		moDataGenericContainer touch = this->input.get(0);
		mapping.push_back(DualPoint(points[this->active_point], moPoint(touch->property("x"), touch->property("y"))));
		this->active_point += 1;
		if (this->active_point == points.length()-1) {
			// OK, we calibrated all the points!
			this->active_point == 0;
			this->property("do_calibration") = false;
		}
		// Perhaps points were added, moved or deleted. If this is the case
		// we have to triangulate again. 
 		if (triangulate) this->triangulate();
		*/
	}
	else {
		// Calibration is done. Just convert the point coordinates.
	}
}


/*
 typedef struct _DualPoint {
 moPoint screen;
 moPoint surface;
 } DualPoint;
 
 typedef struct {
 float alpha;
 float beta;
 float gamma;
 } surf2screen_t;
 
 class Triangle {
 public:
 DualPoint a;
 DualPoint b;
 DualPoint c;
 
 void surfaceToScreen(moPoint p, surf2screen_t *result) {
 assert(result != NULL);
 
 // XXX These should be vectors...
 moPoint abs = {b.screen.x - a.screen.x, b.screen.y - a.screen.y};
 moPoint acs = {c.screen.x - a.screen.x, c.screen.y - a.screen.y};
 result->beta = (p.x - a.screen.x) / float(abs.x);
 result->gamma = (p.y - a.screen.y) / float(acs.y);
 result->alpha = 1.0 - result->beta - result->gamma;
 }
 
 bool collide_point(moPoint point) {
 surf2screen_t params;
 float *p;
 int i;
 
 this->surfaceToScreen(point, &params);
 
 for ( p = (float *)(&params), i = 0; i < 3; i++, p++ ) 
 if (!(0.0 <= *p <= 1.0))
 return false;
 return true;
 }
 };
*/