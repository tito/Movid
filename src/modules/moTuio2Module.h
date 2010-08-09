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


#ifndef MO_TUIO2_MODULE_H
#define MO_TUIO2_MODULE_H

#include <string>
#include "../moModule.h"
#include "../moOSC.h"

class moDataStream;

class moTuio2Module : public moModule {
public:
	moTuio2Module();
	virtual ~moTuio2Module();

	void notifyData(moDataStream *stream);
	void update();

	void start();
	void stop();

private:
	moDataStream *input;
	moOSC *osc;

	bool packBlob(WOscBundle *bundle);
	bool packFiducial(WOscBundle *bundle);
	int probe();

	int frame;

	MODULE_INTERNALS();
};

#endif

