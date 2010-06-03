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

#ifdef NO_LOG

#define LOG_DECLARE(x)
#define LOG(level, x)
#define LOGX(level, x)
#define LOGM(level, x)

#else // NO_LOG

#define LOG_DECLARE(x) \
	static char log_name[] = x;

#define LOG(level, x) do { \
	if ( level <= g_loglevel ) \
		moLogMessage(log_name, __FILE__, __LINE__, level) << x; \
} while (0);

#define LOGX(level, x) do { \
	if ( level <= g_loglevel ) \
		moLogMessage(this->getName(), __FILE__, __LINE__, level) << x; \
} while (0);

#define LOGM(level, x) do { \
	if ( level <= g_loglevel ) \
		moLogMessage(log_name, __FILE__, __LINE__, level) \
		<< "<" << this->property("id").asString() << "> " << x; \
} while(0);

#endif // NO_LOG

#define _LOG_FUNC { \
	if ( this->level <= g_loglevel ) \
		this->os << __n; \
	return *this; \
}

extern int g_loglevel;

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
	static void init(bool use_syslog);
	static void cleanup();
	static void setLogLevel(int n);
	static int getLogLevel();
	static int getSysLogLevel(int n);
	static std::string getLogLevelName(int n);
};

class moLogMessage {
public:
	moLogMessage(std::string name, std::string filename, int line, int level);
	~moLogMessage();

    moLogMessage &operator<<(bool __n) _LOG_FUNC;
	moLogMessage &operator<<(char __n) _LOG_FUNC;
    moLogMessage &operator<<(short __n) _LOG_FUNC;
    moLogMessage &operator<<(int __n) _LOG_FUNC;
    moLogMessage &operator<<(long __n) _LOG_FUNC;
	moLogMessage &operator<<(unsigned char __n) _LOG_FUNC;
    moLogMessage &operator<<(unsigned short __n) _LOG_FUNC;
    moLogMessage &operator<<(unsigned int __n) _LOG_FUNC;
    moLogMessage &operator<<(unsigned long __n) _LOG_FUNC;
    moLogMessage &operator<<(float __n) _LOG_FUNC;
    moLogMessage &operator<<(double __n) _LOG_FUNC;
	moLogMessage &operator<<(std::string __n) _LOG_FUNC;
	moLogMessage &operator<<(const char *__n) _LOG_FUNC;

private:
	std::ostringstream os;
	int level;
};

#endif

