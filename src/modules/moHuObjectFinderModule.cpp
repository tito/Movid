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
#include "moHuObjectFinderModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(HuObjectFinder, "native", "Find objects based on Hu moments");


static void draw_box(IplImage *image, CvBox2D box, double color) {
  CvPoint2D32f boxPoints[4];

  /* CamShift seems to get this backwards */
  //box.angle = -box.angle;

  cvBoxPoints(box, boxPoints);
  cvLineAA(image,
	   cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
	   cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
	   color);
  cvLineAA(image,
	   cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
	   cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
	   color);
  cvLineAA(image,
	   cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
	   cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
	   color);
  cvLineAA(image,
	   cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
	   cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
	   color);
}




void mohuobjectfindermodule_reset_contour(moProperty *property, void *userdata)
{
	moHuObjectFinderModule *module = static_cast<moHuObjectFinderModule *>(userdata);
	assert(userdata != NULL);
	if ( property->asBool() )
		module->stored_contour = NULL;
}

moHuObjectFinderModule::moHuObjectFinderModule() : moImageFilterModule() {
	MODULE_INIT();

	this->storage = cvCreateMemStorage(0);

	this->output_data = new moDataStream("blob");
	this->declareOutput(1, &this->output_data, new moDataStreamInfo(
			"data", "blob", "Data stream with blobs"));

	this->setInputType(0, "IplImage8");
	this->setOutputType(0, "IplImage8");

	this->stored_contour = NULL;

	this->properties["max_size_difference"] = new moProperty(1500);
	this->properties["max_match_score"] = new moProperty(0.8);
	this->properties["min_area"] = new moProperty(150.);
	this->properties["min_area"]->setMin(0.0);
	this->properties["min_area"]->setMax(1000.0);
	this->properties["reset"] = new moProperty(false);
	this->properties["reset"]->addCallback(mohuobjectfindermodule_reset_contour, this);
}

moHuObjectFinderModule::~moHuObjectFinderModule() {
}

void moHuObjectFinderModule::clearRecognizedObjects() {
	moDataGenericList::iterator it;
	for ( it = this->recognized_objects.begin(); it != this->recognized_objects.end(); it++ )
		delete (*it);
	this->recognized_objects.clear();
}

void moHuObjectFinderModule::applyFilter(IplImage *src) {
	this->clearRecognizedObjects();
	cvCopy(src, this->output_buffer);

	CvSeq *contours, *cur_cont;
	double area, matchscore;
	double min_area = this->property("min_area").asDouble();
	double max_diff = this->property("max_size_difference").asInteger();
	double max_score = this->property("max_match_score").asDouble();

	cvFindContours(this->output_buffer, this->storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
	cur_cont = contours;
	CvBox2D mar1, mar2;

	CvSize size = cvGetSize(src);
	moDataGenericContainer *obj;

	while (cur_cont != NULL) {
		cvDrawContours(this->output_buffer, cur_cont, cvScalarAll(255), cvScalarAll(255), 100);
		area = cvContourArea(cur_cont);
		if (area < min_area)
			goto proceed;

		if (this->stored_contour == NULL) {
			std::cout << "Setting contour with area " << area << " ..................................................." << std::endl;
			this->stored_contour = cur_cont;
			// XXX
			break;
		}

		// The Hu moments are unaware of size. We want that, however, so that
		// we can distinguish differently sized objects of similar shape.
		mar1 = cvMinAreaRect2(this->stored_contour, this->storage); 
		mar2 = cvMinAreaRect2(cur_cont, this->storage); 
		draw_box(this->output_buffer, mar1, 255.);
		draw_box(this->output_buffer, mar2, 255.);
		if (fabs(mar1.size.width * mar1.size.height - mar2.size.width * mar2.size.height) > max_diff)
			goto proceed;

		matchscore = cvMatchShapes(this->stored_contour, cur_cont, CV_CONTOURS_MATCH_I2);
		if (matchscore < max_score) {
			std::cout << "MATCH with " << matchscore << " < " << max_score << std::endl;
			obj = new moDataGenericContainer();
			obj->properties["type"] = new moProperty("blob");
			obj->properties["implements"] = new moProperty("object,pos");
			obj->properties["x"] = new moProperty(mar2.center.x / static_cast<float>(size.width));
			obj->properties["y"] = new moProperty(mar2.center.y / static_cast<float>(size.height));
			this->recognized_objects.push_back(obj);
		}

proceed:;	
		cur_cont = cur_cont->h_next;
	}
	this->output_data->push(&this->recognized_objects);
}

