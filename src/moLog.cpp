#include <time.h>
#include <stdlib.h>

#include "moLog.h"

static moLog* instance = NULL;

moLogMessage::moLogMessage(std::string name, std::string filename,
						   int line, int level) {
	char buffer[64];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	strftime(buffer, sizeof(buffer), "%F %H:%M:%S", tmp);

	this->os << buffer << " | ";
	this->os << moLog::getInstance()->getLogLevelName(level) << " | ";
	this->os << (const char *)name.c_str() << " | ";
	this->level = level;
}

moLogMessage::~moLogMessage() {
	if ( this->level <= moLog::getInstance()->getLogLevel() )
		std::cout << this->os.str() << std::endl;
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
