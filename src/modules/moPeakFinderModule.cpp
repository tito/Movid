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

moPeakFinderModule::moPeakFinderModule() : moImageFilterModule(){

	MODULE_INIT();

	// The minimum value a peak must have
	this->properties["min_value"] = new moProperty(0.);
	// The maximum value a peak may have
	this->properties["max_value"] = new moProperty(255.);
	// The maximum number of peaks that you want. Takes the strongest peaks.
	// If this is set to 0, it returns all peaks.
	this->properties["max_peaks"] = new moProperty(1);
	// Avoid duplicate peaks that are close to each other.
	this->properties["merge_distance"] = new moProperty(4.);

//	this->output_data = new moDataStream("GenericBlob");
//	this->output_count = 2;
//	this->output_infos[1] = new moDataStreamInfo("data", "GenericBlob", "Data stream with Blob info");
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

void moPeakFinderModule::findRange(IplImage *src) {
	// Find all pixels whose value is between min_value and max_value
	// and consider it a peak.
	int step = src->widthStep;
	int height = src->height;
	int width = src->width;
	double min = this->property("min_value").asDouble();
	double max = this->property("max_value").asDouble();
	double cur_val;
	char *data = src->imageData;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			cur_val = ((int) data[j * step + i]);
			if ((min < cur_val) && (cur_val < max)) {
				doubleToPoint p;
				p.first = cur_val;
				p.second.x = i;
				p.second.y = j;
				this->peaks.push_back(p);
			}
		}
	}
}

void moPeakFinderModule::findMaxima() {
	// If the max_peaks option was set, we only give back the $max_peaks strongest peaks.
	unsigned int max_peaks = this->property("max_peaks").asInteger();
	if (max_peaks != 0 && max_peaks < this->peaks.size()) {
		std::sort(this->peaks.begin(), this->peaks.end(), _sort_pred);
		// We do so by erasing the weak peaks.
		this->peaks.erase(this->peaks.begin()+max_peaks, this->peaks.end());
	}
}

void moPeakFinderModule::drawPeaks() {
	// Draw the peaks that we've found so we can see if it makes sense
	cvSet(this->output_buffer, cvScalar(0, 0, 0));
	int radius = 5;
	for (unsigned int i = 0; i < this->peaks.size(); i++) {
		// Coordinate system is flipped
		CvPoint p1 = cvPoint(cvRound(this->peaks[i].second.x-radius),
							cvRound(this->peaks[i].second.y-radius));
		CvPoint p2 = cvPoint(cvRound(this->peaks[i].second.x+radius),
							cvRound(this->peaks[i].second.y+radius));
		cvRectangle(this->output_buffer, p1, p2, CV_RGB(255, 255, 255));
	}
}

void moPeakFinderModule::applyFilter(IplImage *src) {
	this->peaks.clear();

	this->findRange(src);
	this->findMaxima();
	this->drawPeaks();

	// Push the peaks as blobs
//	doubleToPoint peak;
//	CvSize size = cvGetSize(src);
//	for (unsigned int i = 0; i < this->peaks.size(); i++) {
//		peak = peaks[i];
//		moDataGenericContainer *blob = new moDataGenericContainer();
//		blob->properties["type"] = new moProperty("blob");
//		blob->properties["x"] = new moProperty(peak.second.x / size.width);
//		blob->properties["y"] = new moProperty(peak.second.y / size.height);
//		// We interpret the peak's value as its dimensions
//		blob->properties["w"] = new moProperty(peak.first);
//		blob->properties["h"] = new moProperty(peak.first);
//		this->blobs->push_back(blob);
//	}
//	this->output_data->push(this->blobs);
}

moDataStream* moPeakFinderModule::getOutput(int n) {
	if ( n == 1 )
		return this->output_data;
	return moImageFilterModule::getOutput(n);
}
