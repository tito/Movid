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

moPeakFinderModule::moPeakFinderModule() : moImageFilterModule(){

	MODULE_INIT();

	this->properties["min_radius"] = new moProperty(5.);
	this->properties["max_radius"] = new moProperty(10.);
}

moPeakFinderModule::~moPeakFinderModule() {
}

void moPeakFinderModule::applyFilter(IplImage *src) {
	double minVal, maxVal;
	CvPoint minLoc, maxLoc;
	cvMinMaxLoc(src, &minVal, &maxVal, &minLoc, &maxLoc);
	std::cout << minVal << "/" << maxVal << std::endl;

	int step = src->widthStep;
	int height = src->height;
	int width = src->width;
	//double scale = this->property("scale").asDouble();
	double min = this->property("min_radius").asDouble();
	double max = this->property("max_radius").asDouble();
	double cur_val;
	char *data = src->imageData;
	std::vector<std::pair<double, moPoint> > peaks;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			//cur_val = ((int) data[i * step + j]) / scale;
			cur_val = ((int) data[i * step + j]);
			if ((min < cur_val) && (cur_val < max)) {
				std::cout << "Cur val: " << cur_val << " is between " << min << " & " << max << " at " << i << ", " << j << std::endl;
				return;
				peaks.push_back(std::pair<double, moPoint>(cur_val, (moPoint) {i, j}));
			}
		}
	}
	for (unsigned int i = 0; i < peaks.size(); i++) {
		// Coordinate system is flipped
		CvPoint p = cvPoint(cvRound(peaks[i].second.x),
							cvRound(peaks[i].second.y));
		int radius = cvRound(peaks[i].first);
		cvCircle(this->output_buffer, p, radius, CV_RGB(255, 255, 255), -1);
	}
}

