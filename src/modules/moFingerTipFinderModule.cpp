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

/*
 * This module can be used to detect a hand in an image.
 * The output is binary image showing the hand's outline, the fingertips (as
 * white circles) and the center of the hand (white rectangle).
 * The following steps are performed:
 *		1. The contours of the segmented hand are found.
 *		   If the area of the contour is not above a certain threshold, the
 *		   contour can be discarded.
 *		2. The convex hull of the contour is determined and convexity defects
 *		   calculated. The defects that are more than a given threshold
 *		   away from the convex hull are considered. Their start and endpoint
 *		   is interpreted as fingertips.
 *		   Since there are 5 fingers visible at max, we consider only the
 *		   strongest defects, of which we only take 5 at a max. (1 def = 2 points)
 *		3. Since there could (should) be double-detections of the index, middle
 *		   and ring finger, we merge fingertips that are in very close proximity
 *		   of each other.
 */


#include <assert.h>
#include "moFingerTipFinderModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(FingerTipFinder, "native", "Module capable of detecting hands in an image. Detection is based on color-segmentation, contour-shape and distance transform. Finds fingertips & centerpoint of the palm.");

struct Point {
	int x;
	int y;
};

typedef std::pair<float, CvConvexityDefect*> depthToDefect;
typedef std::pair<double, Point> doubleToPoint;

template <class T>
bool sort_pred(T left, T right) { return left.first > right.first; }

static bool _in2(std::vector<int> &vec, int e) {
	for (unsigned int i = 0; i < vec.size(); i++)
		if (vec[i] == e)
			return true;
	return false;
}

moFingerTipFinderModule::moFingerTipFinderModule() : moImageFilterModule(){
	MODULE_INIT();

	this->output_data = new moDataStream("blob");
	this->output_count = 2;
	this->output_infos[1] = new moDataStreamInfo("data", "blob", "Data stream with blobs");

	this->storage = cvCreateMemStorage(0);
	this->properties["min_distance"] = new moProperty(20.);
	this->properties["min_area"] = new moProperty(150.);
	this->properties["merge_distance"] = new moProperty(25.);
	this->properties["merge_distance"]->setMin(0.0);
	this->properties["merge_distance"]->setMax(100.0);
	this->properties["adaptive_merge"] = new moProperty(true);
}

moFingerTipFinderModule::~moFingerTipFinderModule() {
	cvReleaseMemStorage(&this->storage);
}

void moFingerTipFinderModule::clearFingertips() {
	moDataGenericList::iterator it;
	for ( it = this->fingertips.begin(); it != this->fingertips.end(); it++ )
		delete (*it);
	this->fingertips.clear();
}

void moFingerTipFinderModule::applyFilter(IplImage *source) {
	CvSeq *contours = NULL;
	IplImage *src;

	this->clearFingertips();

	// First step, search contours
	src = static_cast<IplImage*>(this->input->getData());
	cvCopy(src, this->output_buffer);
	cvFindContours(this->output_buffer, this->storage, &contours, sizeof(CvContour), CV_RETR_CCOMP);
	cvZero(this->output_buffer);

	// No contours found ? Just leave :)
	if ( contours == NULL )
		return;

	// Search palm center, if not found, leave.
	if ( ! this->searchPalmCenter(source, contours) )
		return;

	// Search finger, if any problem, leave.
	if ( ! this->searchFingerTips(source, contours) )
		return;

	// All ok, push fingers on output !
	this->output_data->push(&this->fingertips);
}

bool moFingerTipFinderModule::searchPalmCenter(IplImage *source, CvSeq *contours) {
	CvPoint botleft, topright;
	CvSize size = cvGetSize(source);
	IplImage* filledhand, *dist;
	bool found_center;
	char *data;
	double min, max, cur_val;
	double tolerance = 0.5;
	int peak_x = 0, peak_y = 0;
	unsigned int step, rows, cols, radius;
	int xsum = 0, ysum = 0;
	unsigned int i, j;
	std::vector<doubleToPoint> peaks;
	std::vector<doubleToPoint> strong_peaks;
	moDataGenericContainer *item;
	
	// Draw contours
	filledhand = cvCreateImage(size, IPL_DEPTH_8U, 1);
	dist = cvCreateImage(size, IPL_DEPTH_32F, 1);

	cvDrawContours(filledhand, contours, cvScalarAll(255), cvScalarAll(255), 100, CV_FILLED);
	cvDistTransform(filledhand, dist, CV_DIST_L2, CV_DIST_MASK_PRECISE);
	cvConvertScale(dist, this->output_buffer, 5, 0);

	cvReleaseImage(&filledhand);
	cvReleaseImage(&dist);

	// Find all the peaks in the image that have their value
	// between min_dist_value and max_dist_value
	step = this->output_buffer->widthStep;
	rows = this->output_buffer->height;
	cols = this->output_buffer->width;
	// XXX use proper values...
	min = 0.0;
	max = 500.;
	data = this->output_buffer->imageData;

	for ( i = 0; i < cols; i++ ) {
		for ( j = 0; j < rows; j++ ) {
			cur_val = ((int)data[j * step + i]);
			if ( (min < cur_val) && (cur_val < max) )
				peaks.push_back(doubleToPoint(cur_val, (Point) {i, j}));
		}
	}

	// No center found ? Stop the processing.
	if ( peaks.size() <= 0 )
		return false;

	// We're only interested in the strongest peak. Discard the rest.
	std::sort(peaks.begin(), peaks.end(), sort_pred<doubleToPoint>);

	// Likely we have many strong peaks with a very similar value in
	// close proximity to each other. In that case, take the mean
	// of their position.
	for ( i = 0; i < peaks.size(); i++)
		if (peaks[0].first - peaks[i].first < tolerance)
			strong_peaks.push_back(peaks[i]);

	for ( i = 0; i < strong_peaks.size(); i++ ) {
		xsum += strong_peaks[i].second.x;
		ysum += strong_peaks[i].second.y;
	}

	peak_x = xsum / strong_peaks.size();
	peak_y = ysum / strong_peaks.size();

	// Draw the contour, fingertips and palm center
	if ( this->output->getObserverCount() > 0 ) {
		cvZero(this->output_buffer);
		cvDrawContours(this->output_buffer, contours, cvScalarAll(255), cvScalarAll(255), 100);
		radius = 5;
		if ( found_center ) {
			botleft = cvPoint(cvRound(peak_x-radius), cvRound(peak_y-radius));
			topright = cvPoint(cvRound(peak_x+radius), cvRound(peak_y+radius));
			cvRectangle(this->output_buffer, botleft, topright, CV_RGB(255, 255, 255));
		}
	}

	// Push the center (center will be always 0)
	item = new moDataGenericContainer();
	item->properties["type"] = new moProperty("blob");
	item->properties["implements"] = new moProperty("handcenter,x,y,node");
	item->properties["x"] = new moProperty(peak_x / static_cast<float>(size.width));
	item->properties["y"] = new moProperty(peak_y / static_cast<float>(size.height));
	item->properties["node_id"] = new moProperty(0);
	this->fingertips.push_back(item);

	return true;
}

bool moFingerTipFinderModule::searchFingerTips(IplImage *source, CvSeq *contours) {
	CvSize size = cvGetSize(source);
	CvConvexityDefect *defect;
	CvPoint *p1, *p2, *p;
	CvSeq *max_cont, *cur_cont, *hull, *defects;
	double area, max_area = 0;
	double min_dist, distance, merge_distance;
	float depth;
	std::vector<CvPoint*> points;
	std::vector<depthToDefect> def_depths;
	std::vector<double> distances;
	std::vector<int> suppressed;
	unsigned int i, j, radius;
	moDataGenericContainer *item;

	// Find the exterior contour (i.e. the hand has to be white) that has the greatest area
	max_cont = cur_cont = contours;
	while ( cur_cont != 0 ) {
		area = cvContourArea(cur_cont);
		area *= area;
		if ( area >= max_area ) {
			max_area = area;
			max_cont = cur_cont;
		}
		cur_cont = cur_cont->h_next;
	}

	// Ignore contours whose area is too small, they're likely not hands anyway
	if ( max_area < this->property("min_area").asDouble() )
		return false;
	contours = max_cont;

	// Compute the convex hull of the contour
	hull = cvConvexHull2(contours, contours->storage, CV_CLOCKWISE, 0);

	// Compute the convexity defects of the convex contour with respect to the convex hull.
	// The fingertips are at the start and endpoints of the defects
	defects = cvConvexityDefects(contours, hull, contours->storage);

	// Sort the defects based on their distance from the convex hull
	for( i = 0; i < static_cast<unsigned int>(defects->total); i++ ) {
		defect = reinterpret_cast<CvConvexityDefect*>(cvGetSeqElem (defects, i));
		def_depths.push_back(depthToDefect(defect->depth, defect));
	}
	std::sort(def_depths.begin(), def_depths.end(), sort_pred<depthToDefect>);

	// Find the start and end points of the 4 best defects
	for ( i = 0; i < def_depths.size(); i++ ) {
		// 5 Fingers max, i.e. 4 defects max.
		if (i == 4)
			break;
		depth = def_depths[i].first;
		min_dist = this->property("min_distance").asDouble();
		if ( min_dist > depth )
			continue;
		defect = def_depths[i].second;
		points.push_back(defect->start);
		points.push_back(defect->end);
	}

	// Merge almost coinciding points
	merge_distance = this->property("merge_distance").asDouble();
	for ( i = 0; i < points.size(); i++ ) {
		p1 = points[i];
		for ( j = i + 1; j < points.size(); j++ ) {
			if ( _in2(suppressed, j) )
				continue;
			p2 = points[j];
			distance = sqrt(pow(p1->x - p2->x, 2.0) + pow(p1->y - p2->y, 2.0));
			distances.push_back(distance);
			if ( distance <= merge_distance )
				suppressed.push_back(j);
		}
	}

	// If there were more or less than 5 fingers detected, we should adapt the merge distance
	// Heuristic: If the hand was somewhat spread out, there were >= 18 distances computed
	if ( this->property("adaptive_merge").asBool()
		 && 18 <= distances.size()
		 && (points.size() - suppressed.size()) != 5
	) {
		std::sort(distances.begin(), distances.end());

		// Now we want to set the merge_distance to a bit more than the
		// largest of the three smallest distances.			
		this->property("merge_distance").set(distances[2] + 1.);
	}

	// Draw fingers
	if ( this->output->getObserverCount() > 0 ) {
		radius = cvRound(10);
		for ( i = 0; i < points.size(); i++ ) {
			if ( _in2(suppressed, i) )
				continue;
			cvCircle(this->output_buffer, *points[i],
					radius, CV_RGB(255, 255, 255), -1);
		}
	}

	// Push fingers
	for ( i = 0; i < points.size(); i++ ) {
		if ( _in2(suppressed, i) )
			continue;

		p = points[i];
		item = new moDataGenericContainer();
		item->properties["type"] = new moProperty("blob");
		item->properties["implements"] = new moProperty("fingertip,x,y,node");
		item->properties["x"] = new moProperty(p->x / static_cast<float>(size.width));
		item->properties["y"] = new moProperty(p->y / static_cast<float>(size.height));
		item->properties["node_id"] = new moProperty(static_cast<int>(i + 1));
		item->properties["parent_node_id"] = new moProperty(0);
		this->fingertips.push_back(item);
	}

	return true;
}

moDataStream* moFingerTipFinderModule::getOutput(int n) {
	if ( n == 1 )
		return this->output_data;
	return moImageFilterModule::getOutput(n);
}

