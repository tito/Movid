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


#ifndef MO_JUSTIFY_MODULE_H
#define MO_JUSTIFY_MODULE_H

#include "../moModule.h"

class moJustifyModule : public moModule{
public:
	moJustifyModule();
	virtual ~moJustifyModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);
	virtual void notifyData(moDataStream *input);

	void update();

private:
	moDataStream *input;
	moDataStream *output;
	moDataGenericList blobs;

	MODULE_INTERNALS();
};

#endif

