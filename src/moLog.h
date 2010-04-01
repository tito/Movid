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


#ifndef MO_LOG_H
#define MO_LOG_H

#include <sstream>
#include <iostream>
#include <string>

#define LOG_DECLARE(x) \
	static char log_name[] = x;


#define LOG(x) moLogMessage(log_name, __FILE__, __LINE__, x).get()
#define LOGX(x) moLogMessage(this->getName(), __FILE__, __LINE__, x).get()
#define LOGM(x) LOG(x) << "<" << this->property("id").asString() << "> "

enum {
	MO_CRITICAL		= 0,
	MO_ERROR		= 1,
	MO_WARNING		= 2,
	MO_INFO			= 3,
	MO_DEBUG		= 4,
	MO_TRACE		= 5,
};

class moLogMessage {
public:
	moLogMessage(std::string name, std::string filename, int line, int level);
	~moLogMessage();

	std::ostringstream &get();

private:
	std::ostringstream os;
	int level;
};

class moLog {
public:
	static moLog *getInstance();
	void setLogLevel(int n);
	int getLogLevel();
	std::string getLogLevelName(int n);

private:
	moLog();
	~moLog();
	int loglevel;
};

#endif

