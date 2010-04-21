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
#include "sstream"
#include "moCalibrationModule.h"
#include "../moLog.h"
#include <stdio.h>

// XXX Desc
MODULE_DECLARE(Calibration, "native", "Calibration");

moCalibrationModule::moCalibrationModule() : moModule(MO_MODULE_INPUT | MO_MODULE_OUTPUT | MO_MODULE_GUI, 1, 1){

	MODULE_INIT();

	this->properties["rows"] = new moProperty(3);
	this->properties["cols"] = new moProperty(3);
	this->properties["screenPoints"] = new moProperty(moPointList());
	this->properties["calibrate"] = new moProperty(true);
	// XXX
	this->retriangulate = true;
	this->rect = cvRect(0, 0, 5000, 5000);
	this->storage = cvCreateMemStorage(0);
	this->active_point = 0;
}

moCalibrationModule::~moCalibrationModule() {
}

void moCalibrationModule::guiFeedback(const std::string& type, double x, double y) {
	// just a temporary ui playground
	// draw only one rect that follow the mouse !
	std::ostringstream oss;
	this->gui.clear();
	this->gui.push_back("viewport 100 100");
	this->gui.push_back("color 255 255 255");
	oss << "rect " << x - 10 << " " << y - 10 << " 20 20";
	this->gui.push_back(oss.str());
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

void moCalibrationModule::update() {
	this->input->lock();	
	
	bool calibrate = this->property("calibrate").asBool();
	if (calibrate) {
		moDataGenericList *blobs = static_cast<moDataGenericList*>(input->getData());
		std::cout << "#Blobs in frame: " << blobs->size() << std::endl;
		// We only calibrate the current point if there is an unambiguous amount of touches
		if (blobs->size() == 1) {
			// We now want to assign each point its coordinates on the touch surface
			moDataGenericContainer *touch = (*blobs)[0];
			// TODO While calibrating, this list MUST NOT change!
			moPointList screenPoints = this->property("screenPoints").asPointList();
			if (this->active_point == screenPoints.size()) {
				// We have calibrated all points, so we're done.
				std::cout << "Calibration complete!" << std::endl;
				this->active_point = 0;
				this->property("calibrate").set(false);
				this->input->unlock();
				return;
			}
			std::cout << "# of screenPoints: " << screenPoints.size() << std::endl;
			std::cout << "Processing point #" << this->active_point << std::endl;
			// We're starting calibration again, so discard all old calibration results
			if (this->active_point == 0) this->surfacePoints.clear();
			// screenPoints and corresponding surfacePoint have the same index in their respective vector
			moPoint surfacePoint = {touch->properties["x"]->asDouble(),
									touch->properties["y"]->asDouble()};
			this->surfacePoints.push_back(surfacePoint);
			
			moPoint p = screenPoints[this->active_point];
			std::cout << "(" << p.x << ", " << p.y << ") is mapped to";
			std::cout << " (" << surfacePoint.x << ", " << surfacePoint.y << ")." << std::endl;
			// Proceed with the next grid point
			this->active_point++;

			// Perhaps points were added, moved or deleted. If this is the case
			// we have to triangulate again. 
			if (this->retriangulate) this->triangulate();
		}
	}
	else {
		// Calibration & triangulation is done. Just convert the point coordinates.
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

