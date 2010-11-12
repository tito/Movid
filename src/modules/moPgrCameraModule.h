/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.and Aras (arasbm@gmail.com)  All rights reserved.
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


#ifndef MO_PGR_H
#define MO_PGR_H

#include "../moModule.h"
#include "/usr/include/flycapture/FlyCapture2.h"
#include "highgui.h"

using namespace FlyCapture2;

class moDataStream;

class moPgrCameraModule : public moModule {
public:
	moPgrCameraModule(); 
	virtual ~moPgrCameraModule();

	virtual void start();
	virtual void stop();
	virtual void update();
	virtual void poll();

private:
	Camera camera;
        PGRGuid pgrGuid;	
	moDataStream *stream;
	moDataStream *monochrome;
        IplImage* ConvertImageToOpenCV(Image* pImage);

	MODULE_INTERNALS();
};

#endif

