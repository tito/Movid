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


#define LOG(x) moLogMessage(log_name, __FILE__, __LINE__, x)
#define LOGX(x) moLogMessage(this->getName(), __FILE__, __LINE__, x)
#define LOGM(x) LOG(x) << "<" << this->property("id").asString() << "> "

#define _LOG_FUNC { \
	if ( this->level <= moLog::getInstance()->getLogLevel() ) \
		this->os << __n; \
	return *this; \
}


enum {
	MO_CRITICAL		= 0,
	MO_ERROR		= 1,
	MO_WARNING		= 2,
	MO_INFO			= 3,
	MO_DEBUG		= 4,
	MO_TRACE		= 5,
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

class moLogMessage {
public:
	moLogMessage(std::string name, std::string filename, int line, int level);
	~moLogMessage();

	moLogMessage &operator<<(unsigned char __n) _LOG_FUNC;
	moLogMessage &operator<<(char __n) _LOG_FUNC;
	moLogMessage &operator<<(size_t __n) _LOG_FUNC;
	moLogMessage &operator<<(ssize_t __n) _LOG_FUNC;
    moLogMessage &operator<<(long __n) _LOG_FUNC;
    moLogMessage &operator<<(unsigned long __n) _LOG_FUNC;
    moLogMessage &operator<<(bool __n) _LOG_FUNC;
    moLogMessage &operator<<(short __n) _LOG_FUNC;
    moLogMessage &operator<<(unsigned short __n) _LOG_FUNC;
    moLogMessage &operator<<(float __n) _LOG_FUNC;
    moLogMessage &operator<<(double __n) _LOG_FUNC;
	moLogMessage &operator<<(std::string __n) _LOG_FUNC;
	moLogMessage &operator<<(const char *__n) _LOG_FUNC;

private:
	std::ostringstream os;
	int level;
};

#endif

