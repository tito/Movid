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


#ifndef MO_OSC_H
#define MO_OSC_H

#include "WOscBundle.h"
#include "WOscMessage.h"
#include <iostream>

class moOSC {
public:
	moOSC(const std::string &ip, unsigned short port);
	virtual ~moOSC();
	void send(WOscMessage *msg);
	void send(WOscBundle *msg);

private:
	void init();

	int sock;
	std::string ip;
	unsigned short port;
};

#endif

