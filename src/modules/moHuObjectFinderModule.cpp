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
#include <math.h>
#include <fstream>
#include <iostream>
#include "moHuObjectFinderModule.h"
#include "../moLog.h"
#include "cv.h"
// Need to get MAX_FIDUCIALS from here:
#include "moFiducialFinderModule.h"

#define PI 3.14159265

MODULE_DECLARE(HuObjectFinder, "native", "Find objects based on Hu moments");


//static void draw_box(IplImage *image, CvBox2D box, double color) {
//  CvPoint2D32f boxPoints[4];
//
//  /* CamShift seems to get this backwards */
//  //box.angle = -box.angle;
//
//  cvBoxPoints(box, boxPoints);
//  cvLineAA(image,
//	   cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
//	   cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
//	   color);
//  cvLineAA(image,
//	   cvPoint((int)boxPoints[1].x, (int)boxPoints[1].y),
//	   cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
//	   color);
//  cvLineAA(image,
//	   cvPoint((int)boxPoints[2].x, (int)boxPoints[2].y),
//	   cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
//	   color);
//  cvLineAA(image,
//	   cvPoint((int)boxPoints[3].x, (int)boxPoints[3].y),
//	   cvPoint((int)boxPoints[0].x, (int)boxPoints[0].y),
//	   color);
//}


void mohuobjectfindermodule_register_object(moProperty *property, void *userdata)
{
	moHuObjectFinderModule *module = static_cast<moHuObjectFinderModule *>(userdata);
	assert(userdata != NULL);
	if (module->contours_restored)
		// Simple trick. This is true only if applyFilter() has been called once.
		// We do this to prevent this callback to be fired when starting from dumped preset.
		module->stored_contours.push_back(NULL);
}

moHuObjectFinderModule::moHuObjectFinderModule() : moImageFilterModule() {
	MODULE_INIT();

	this->storage = cvCreateMemStorage(0);

	this->output_data = new moDataStream("blob");
	this->declareOutput(1, &this->output_data, new moDataStreamInfo(
			"data", "blob", "Data stream with blobs"));

	this->setInputType(0, "IplImage8");
	this->setOutputType(0, "IplImage8");


	// In order to prevent a clash with FiducialFinder's IDs, you can ask
	// HuObjectFinder to start counting objects at a certain ID.
	this->properties["min_id"] = new moProperty(MAX_FIDUCIALS + 1);
	this->properties["max_size_difference"] = new moProperty(1500);
	this->properties["max_match_score"] = new moProperty(0.5);
	this->properties["min_area"] = new moProperty(150.);
	this->properties["min_area"]->setMin(0.0);
	this->properties["min_area"]->setMax(1000.0);
	// FIXME How to do a proper button?
	this->properties["register"] = new moProperty(false);
	this->properties["register"]->addCallback(mohuobjectfindermodule_register_object, this);
	this->properties["consider_holes"] = new moProperty(false);
	this->properties["consider_shape"] = new moProperty(true);
	this->contours_restored = false;
}

moHuObjectFinderModule::~moHuObjectFinderModule() {
}

void moHuObjectFinderModule::serializeContour(CvSeq *cont) {
	char *tmp = tmpnam(NULL);
	cvSave(tmp, cont);

	FILE *fp;
	long len;
	char *buf;
	fp = fopen(tmp, "rb");
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf = (char *) malloc(len+1);
	if (buf == NULL)
		return;
	fread(buf, len, 1, fp);
	buf[len] = '\0';
	fclose(fp);

	int cur_num = this->property("min_id").asInteger() + this->stored_contours.size();
	std::ostringstream prop_name;
	prop_name << "stored_contour_" << cur_num;
	this->properties[prop_name.str()] = new moProperty("");
	this->properties[prop_name.str()]->setText(true);
	this->properties[prop_name.str()]->set(buf);

	free(buf);
}

void moHuObjectFinderModule::clearRecognizedObjects() {
	moDataGenericList::iterator it;
	for ( it = this->recognized_objects.begin(); it != this->recognized_objects.end(); it++ )
		delete (*it);
	this->recognized_objects.clear();
}

inline float moHuObjectFinderModule::boundingBoxCheck(CvSeq *cont1, CvSeq *cont2, CvBox2D &mar) {
	// The Hu moments are unaware of size. We want that, however, so that
	// we can distinguish differently sized objects of similar shape.
	CvBox2D mar1 = cvMinAreaRect2(cont1, this->storage);
	mar = cvMinAreaRect2(cont2, this->storage);
	return fabs(mar1.size.width * mar1.size.height - mar.size.width * mar.size.height);
}

inline int countHolesInContour(CvSeq *contour) {
	// Only works with CV_RETR_CCOMP
	int count = 0;
	CvSeq *cur_hole = contour->v_next;
	while (cur_hole != NULL) {
		count++;
		cur_hole = cur_hole->h_next;	
	}
	return count;
}

inline int moHuObjectFinderModule::findMatchingShape(CvSeq *cont, CvBox2D &mar) {
	// Checks if cont matches any of the stored contours.
	// Returns index of the most similar contour, or -1 if none was found.
	std::vector<CvSeq*>::iterator it;
	int index = -1;
	int min_index = -1;
	double matchscore;
	double min_score = this->property("max_match_score").asDouble();
	bool consider_holes = this->properties["consider_holes"]->asBool();
	bool consider_shape = this->properties["consider_shape"]->asBool();
	double min_diff = this->property("max_size_difference").asDouble();

	for (it = this->stored_contours.begin(); it != this->stored_contours.end(); it++) {
		index++;

		if (*it == NULL)
			continue;

		if (consider_holes && (countHolesInContour(*it) != countHolesInContour(cont)))
			continue;

		// Do early bounding box check and continue if the sizes differ too much
		// OR if a previous shape was less different.
		matchscore = this->boundingBoxCheck(*it, cont, mar);
		if (matchscore >= min_diff)
			continue;
		min_diff = matchscore;
		min_index = index;

		if (consider_shape) {
			// Bounding boxes (and optionally holes) were OK. Use shape features as main
			// criteria to distinguish between shapes. Override previous BB matchscore/index.
			matchscore = cvMatchShapes(*it, cont, CV_CONTOURS_MATCH_I2);
			if (matchscore < min_score) {
				min_score = matchscore;
				min_index = index;
			}
		}
	}

	return min_index;
}

void moHuObjectFinderModule::applyFilter(IplImage *src) {
	this->clearRecognizedObjects();

	// If this is the first time that applyFilter is called, restore contours from preset.
	if (!this->contours_restored) {
		// XXX fix +1
		int cur_id = this->properties["min_id"]->asInteger() + 1;
		std::ostringstream cur_prop_name;
		cur_prop_name << "stored_contour_" << cur_id;
		char *tmp;
		std::ofstream tmpfile;
		CvSeq *cur_cont;
		while (this->properties.count(cur_prop_name.str()) > 0) {
			moProperty *cur_prop = this->properties[cur_prop_name.str()];
			// Unserialize
			tmp = tmpnam(NULL);
			tmpfile.open(tmp);
			tmpfile << cur_prop->asString() << '\n';
			tmpfile.close();
			cur_cont = (CvSeq*) cvLoad(tmp, this->storage);
			this->stored_contours.push_back(cur_cont);

			cur_id++;
			cur_prop_name.str("");
			cur_prop_name << "stored_contour_" << cur_id;
		}
		this->contours_restored = true;
	}

	cvCopy(src, this->output_buffer);

	CvSeq *contours, *cur_cont;
	double area;
	double min_area = this->property("min_area").asDouble();

	cvFindContours(this->output_buffer, this->storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
	cur_cont = contours;

	CvSize size = cvGetSize(src);
	moDataGenericContainer *obj;
	CvBox2D mar;

	int matched_index, min_id;
	CvMoments moments;
	double cogx, cogy, dx, dy, angle, len, deg;
	double m00, m10, m01;
	float w = static_cast<float>(size.width);
	float h = static_cast<float>(size.height);

	// XXX Do we want to be able to use the same object more than once? Currently the code allows that...
	// Consider all the contours that are in the current frame...
	while (cur_cont != NULL) {
		cvDrawContours(this->output_buffer, cur_cont, cvScalarAll(255), cvScalarAll(255), 100);
		area = cvContourArea(cur_cont);
		if (area > min_area) {
			if (this->stored_contours.size() && (this->stored_contours.back() == NULL)) {
				this->stored_contours.pop_back();
				if (this->findMatchingShape(cur_cont, mar) < 0) {
					this->stored_contours.push_back(cur_cont);
					this->serializeContour(cur_cont);
				}
				break;
			}

			matched_index = this->findMatchingShape(cur_cont, mar);
			min_id = this->property("min_id").asInteger();
			if (matched_index >= 0) {
				obj = new moDataGenericContainer();
				obj->properties["type"] = new moProperty("blob");
				obj->properties["implements"] = new moProperty("markerlessobject,pos");
				obj->properties["x"] = new moProperty(mar.center.x / w);
				obj->properties["y"] = new moProperty(mar.center.y / h);

				// Compute the angle as the angle of the vector from center of gravity to BB center.
				cvContourMoments(cur_cont, &moments);
				m00 = cvGetSpatialMoment(&moments, 0, 0);
				m10 = cvGetSpatialMoment(&moments, 1, 0);
				m01 = cvGetSpatialMoment(&moments, 0, 1);
				cogx = m10 / m00;
				cogy = m01 / m00;
				dy = mar.center.y - cogy;
				dx = mar.center.x - cogx;
				// XXX optim
				len = sqrt(pow(dx, 2) + pow(dy, 2));
				angle = abs(dx) <= 0.001 ? atan(dy / dx) : PI/2.;
				deg = angle * (180 / PI);
				//std::cout << "DX: " << dx << " DY: " << dy << " LEN: " << len << " ANGLE RAD: " << angle << " ANGLE DEG: " << deg << std::endl;

				// Radians, so 0..2PI!
				obj->properties["angle"] = new moProperty(angle);
				obj->properties["fiducial_id"] = new moProperty(min_id + matched_index);
				this->recognized_objects.push_back(obj);
			}
		}
		cur_cont = cur_cont->h_next;
	}
	this->output_data->push(&this->recognized_objects);
}

