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


#ifndef MO_IMAGE_FILTER_MODULE_H
#define MO_IMAGE_FILTER_MODULE_H

#include <string>
#include "cv.h"
#include "../moModule.h"
#include "../moDataStream.h"

class moImageFilterModule : public moModule {
	
public:	
	moImageFilterModule();
	virtual ~moImageFilterModule();

	virtual void notifyData(moDataStream *source);
	virtual void update();
	virtual void stop();
	
protected:
	moDataStream* input;
	moDataStream* output;
	moDataStream* output_dispatcher;
	IplImage* output_buffer;
	
	virtual void applyFilter(IplImage *)=0;
	virtual void allocateBuffers();
	virtual bool needBufferAllocation();
	
	bool need_update;
};

class moImageFilterModule8 : public moImageFilterModule {
public:
	moImageFilterModule8();
	virtual ~moImageFilterModule8();

	virtual void allocateBuffers();
	virtual bool needBufferAllocation();

protected:
	moDataStream* output8;
};

#endif
