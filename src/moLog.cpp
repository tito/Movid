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

static moLog* instance = NULL;
static pt::mutex(logmtx);

moLogMessage::moLogMessage(std::string name, std::string filename,
						   int line, int level) {
	char buffer[64];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	strftime(buffer, sizeof(buffer), "%H:%M:%S", tmp);

	this->os << buffer << " | ";
	this->os << moLog::getInstance()->getLogLevelName(level) << " | ";
	this->os << (const char *)name.c_str() << " | ";
	this->level = level;
}

moLogMessage::~moLogMessage() {
	if ( this->level <= moLog::getInstance()->getLogLevel() ) {
		logmtx.lock();
		std::cout << this->os.str() << std::endl;
		logmtx.unlock();
	}
}

std::ostringstream &moLogMessage::get() {
	return this->os;
}


moLog::moLog() {
	this->loglevel = MO_INFO;
	if ( getenv("MO_DEBUG") )
		this->loglevel = MO_DEBUG;
	if ( getenv("MO_TRACE") )
		this->loglevel = MO_TRACE;
}

moLog::~moLog() {
}

moLog *moLog::getInstance() {
	if ( instance == NULL )
		instance = new moLog();
	return instance;
}

int moLog::getLogLevel() {
	return this->loglevel;
}

void moLog::setLogLevel(int n) {
	this->loglevel = n;
}

std::string moLog::getLogLevelName(int n) {
	switch ( n ) {
		case MO_CRITICAL:	return "Critical";
		case MO_ERROR:		return "Error";
		case MO_WARNING:	return "Warning";
		case MO_INFO:		return "Info";
		case MO_DEBUG:		return "Debug";
		case MO_TRACE:		return "Trace";
	}
	return "Unknown";
}
