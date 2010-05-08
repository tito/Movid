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
#include "moPeakFinderModule.h"
#include "../moLog.h"
#include "cv.h"

MODULE_DECLARE(PeakFinder, "native", "PeakFinder Description");

typedef std::pair<double, moPoint> doubleToPoint;

moPeakFinderModule::moPeakFinderModule() : moImageFilterModule(){

	MODULE_INIT();

	this->properties["min_radius"] = new moProperty(5.);
	this->properties["max_radius"] = new moProperty(10.);
	this->properties["max_peaks"] = new moProperty(10);
}

moPeakFinderModule::~moPeakFinderModule() {
}

bool _sort_pred(const doubleToPoint &left, const doubleToPoint &right) {
	return left.first > right.first;
}

void moPeakFinderModule::applyFilter(IplImage *src) {
	int step = src->widthStep;
	int height = src->height;
	int width = src->width;
	//double scale = this->property("scale").asDouble();
	double min = this->property("min_radius").asDouble();
	double max = this->property("max_radius").asDouble();
	double cur_val;
	char *data = src->imageData;
	std::vector<doubleToPoint> peaks;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			cur_val = ((int) data[j * step + i]);
			if ((min < cur_val) && (cur_val < max)) {
				peaks.push_back(doubleToPoint(cur_val, (moPoint) {i, j}));
			}
		}
	}

	// If the max_peaks option was set, we only give back the $max_peaks strongest peaks.
	int max_peaks = this->property("max_peaks").asInteger();
	if (max_peaks != 0 && max_peaks < peaks.size()) {
		std::sort(peaks.begin(), peaks.end(), _sort_pred);
		// We do so by erasing the weak peaks.
		peaks.erase(peaks.begin()+max_peaks, peaks.end());
	}

	// Draw the peaks that we've found so we can see if it makes sense
	cvSet(this->output_buffer, cvScalar(0, 0, 0));
	//std::cout << "---" << std::endl;
	for (unsigned int i = 0; i < peaks.size(); i++) {
		//std::cout << peaks[i].first << std::endl;
		// Coordinate system is flipped
		CvPoint p = cvPoint(cvRound(peaks[i].second.x),
							cvRound(peaks[i].second.y));
		int radius = cvRound(peaks[i].first);
		cvCircle(this->output_buffer, p, radius, CV_RGB(255, 255, 255), -1);
	}
}

