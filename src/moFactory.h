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


#ifndef MO_FACTORY_H
#define MO_FACTORY_H

#include <vector>
#include <map>
#include <string>

#include "moModule.h"

typedef moModule *(*moFactoryCreateCallback)();

class moFactory {
public:
	~moFactory();

	static moFactory *getInstance();
	static void init();
	static void cleanup();

	void registerModule(const std::string &name, moFactoryCreateCallback callback);

	moModule *create(const std::string &name);
	std::vector<std::string> list();

protected:
	moFactory();

	std::map<std::string, moFactoryCreateCallback> database;
};

#endif

