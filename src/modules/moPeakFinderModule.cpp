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


#include <math.h>
#include <assert.h>
#include "moPeakFinderModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(PeakFinder, "native", "PeakFinder Description");

typedef std::pair<double, moPoint> doubleToPoint;

moPeakFinderModule::moPeakFinderModule() : moImageFilterModule(){

	MODULE_INIT();

	// The minimum value a peak must have
	this->properties["min_value"] = new moProperty(5.);
	// The maximum value a peak may have
	this->properties["max_value"] = new moProperty(10.);
	// The maximum number of peaks that you want. Takes the strongest peaks.
	// If this is set to 0, it returns all peaks.
	this->properties["max_peaks"] = new moProperty(10);
	// Avoid duplicate peaks that are close to each other.
	this->properties["merge_distance"] = new moProperty(4.);
}

moPeakFinderModule::~moPeakFinderModule() {
}

bool _sort_pred(const doubleToPoint &left, const doubleToPoint &right) {
	return left.first > right.first;
}

bool _in(std::vector<int> &vec, int e) {
	for (unsigned int i = 0; i < vec.size(); i++) {
		if (vec[i] == e) return true;
	}
	return false;
}

void moPeakFinderModule::applyFilter(IplImage *src) {
	LOG(MO_TRACE) << "----------------------------------------------- BEGINNING FRAME";
	int step = src->widthStep;
	int height = src->height;
	int width = src->width;
	double min = this->property("min_value").asDouble();
	double max = this->property("max_value").asDouble();
	double cur_val;
	char *data = src->imageData;
	std::vector<doubleToPoint> peaks;

	// Find all pixels whose value is between min_value and max_value
	// and consider it a peak.
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			cur_val = ((int) data[j * step + i]);
			if ((min < cur_val) && (cur_val < max)) {
				peaks.push_back(doubleToPoint(cur_val, (moPoint) {i, j}));
			}
		}
	}

	// Remove duplicate peaks.
	double distance;
	double merge_distance = this->property("merge_distance").asDouble();
	doubleToPoint p1, p2;
	std::vector<int> suppressed;
	for (unsigned int i = 0; i < peaks.size(); i++) {
		p1 = peaks[i];
		for (unsigned int j = i+1; j < peaks.size(); j++) {
			if (i == j) continue;
			if (_in(suppressed, j)) continue;
			LOG(MO_TRACE) << "---------";
			LOG(MO_TRACE) << "i/j/size: " << i << ", " << j << ", " << peaks.size();
			p2 = peaks[j];
			distance = sqrt(pow(p1.second.x - p2.second.x, 2) + pow(p1.second.y - p2.second.y, 2));
			LOG(MO_TRACE) << "distance((" << p1.second.x << "/" << p1.second.y << "), (" << p2.second.x << "/" << p2.second.y << ") == " << distance;
			if (distance <= merge_distance) {
				LOG(MO_TRACE) << "Suppressing " << j;
				suppressed.push_back(j);
			}
		}
	}
	if (suppressed.size() > 0) {
		std::vector<doubleToPoint> good_peaks;
		unsigned int orig_size = peaks.size();
		for (unsigned int i = 0; i < suppressed.size(); i++) {
			if (!_in(suppressed, i)) good_peaks.push_back(peaks[i]);
		}
		peaks = good_peaks;
	}

	// If the max_peaks option was set, we only give back the $max_peaks strongest peaks.
	unsigned int max_peaks = this->property("max_peaks").asInteger();
	if (max_peaks != 0 && max_peaks < peaks.size()) {
		std::sort(peaks.begin(), peaks.end(), _sort_pred);
		// We do so by erasing the weak peaks.
		peaks.erase(peaks.begin()+max_peaks, peaks.end());
	}

	// Draw the peaks that we've found so we can see if it makes sense
	cvSet(this->output_buffer, cvScalar(0, 0, 0));
	LOG(MO_TRACE) << "--- " << peaks.size() << " peaks";
	int radius;
	for (unsigned int i = 0; i < peaks.size(); i++) {
		LOG(MO_TRACE) << peaks[i].first << " @ " << peaks[i].second.x << "/" << peaks[i].second.y << std::endl;
		// Coordinate system is flipped
		CvPoint p = cvPoint(cvRound(peaks[i].second.x),
							cvRound(peaks[i].second.y));
		radius = cvRound(peaks[i].first);
		cvCircle(this->output_buffer, p, radius, CV_RGB(255, 255, 255), -1);
	}
}

