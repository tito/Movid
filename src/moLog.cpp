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


#include <time.h>
#include <stdlib.h>

#include "moLog.h"
#include "pasync.h"

#ifndef WIN32
#include <syslog.h>
#endif

static pt::mutex(logmtx);

int g_loglevel = MO_INFO;
bool g_use_syslog = false;

moLogMessage::moLogMessage(std::string name, std::string filename,
						   int line, int level) {
	char buffer[64];
	time_t t;
	struct tm *tmp;

	if (! g_use_syslog) {
		t = time(NULL);
		tmp = localtime(&t);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", tmp);

		this->os << buffer << " | ";
	}
	this->os << moLog::getLogLevelName(level) << " | ";
	this->os << (const char *)name.c_str() << " | ";
	this->level = level;
}

moLogMessage::~moLogMessage() {
	if (this->level <= g_loglevel) {
		logmtx.lock();
#ifndef WIN32
		if (g_use_syslog) {
			syslog(LOG_USER | moLog::getSysLogLevel(this->level), "%s", this->os.str().c_str());
		} else {
			std::cout << this->os.str() << std::endl;
		}
#else
		std::cout << this->os.str() << std::endl;
#endif
		logmtx.unlock();
	}
}


void moLog::init(bool use_syslog) {
#ifndef WIN32
	g_use_syslog = use_syslog;
#endif
	g_loglevel = MO_INFO;
	if (getenv("MO_DEBUG"))
		g_loglevel = MO_DEBUG;
	if (getenv("MO_TRACE"))
		g_loglevel = MO_TRACE;
}

void moLog::cleanup() {
}

int moLog::getLogLevel() {
	return g_loglevel;
}

void moLog::setLogLevel(int n) {
	g_loglevel = n;
}

int moLog::getSysLogLevel(int n) {
	switch (n) {
		case MO_CRITICAL:	return LOG_CRIT;
		case MO_ERROR:		return LOG_ERR;
		case MO_WARNING:	return LOG_WARNING;
		case MO_INFO:		return LOG_INFO;
		case MO_DEBUG:		return LOG_DEBUG;
		default:		return 0;
	}
}

std::string moLog::getLogLevelName(int n) {
	switch (n) {
		case MO_CRITICAL:	return "Critical";
		case MO_ERROR:		return "Error";
		case MO_WARNING:	return "Warning";
		case MO_INFO:		return "Info";
		case MO_DEBUG:		return "Debug";
		case MO_TRACE:		return "Trace";
	}
	return "Unknown";
}
