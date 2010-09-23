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


#ifndef MO_HUOBJECTFINDER_MODULE_H
#define MO_HUOBJECTFINDER_MODULE_H

#include "../moDataGenericContainer.h"
#include "moImageFilterModule.h"

#define PI 3.14159265
typedef std::pair<int, bool> match;

class moHuObjectFinderModule : public moImageFilterModule {
public:
	moHuObjectFinderModule();
	virtual ~moHuObjectFinderModule();

	std::vector<CvSeq*> stored_contours;
	bool contours_restored;
	
protected:
	void applyFilter(IplImage *);
	void clearRecognizedObjects();
	void serializeContour(CvSeq*);
	float boundingBoxCheck(CvSeq *cont1, CvSeq *cont2, CvBox2D &mar);
	match findMatchingShape(CvSeq *cont, CvBox2D &mar);
	moDataGenericList recognized_objects;
	moDataStream *output_data;

	CvMemStorage *storage;

	MODULE_INTERNALS();
};

#endif

