#include <time.h>
#include <stdlib.h>

#include "otLog.h"

static otLog* instance = NULL;

otLogMessage::otLogMessage(std::string name, std::string filename,
						   int line, int level) {
	char buffer[64];
	time_t t;
	struct tm *tmp;

	t = time(NULL);
	tmp = localtime(&t);
	strftime(buffer, sizeof(buffer), "%F %H:%M:%S", tmp);

	this->os << buffer << " | ";
	this->os << otLog::getInstance()->getLogLevelName(level) << " | ";
	this->os << (const char *)name.c_str() << " | ";
	this->level = level;
}

otLogMessage::~otLogMessage() {
	if ( this->level <= otLog::getInstance()->getLogLevel() )
		std::cout << this->os.str() << std::endl;
}

std::ostringstream &otLogMessage::get() {
	return this->os;
}


otLog::otLog() {
	this->loglevel = INFO;
	if ( getenv("OT_DEBUG") )
		this->loglevel = DEBUG;
	if ( getenv("OT_TRACE") )
		this->loglevel = TRACE;
}

otLog::~otLog() {
}

otLog *otLog::getInstance() {
	if ( instance == NULL )
		instance = new otLog();
	return instance;
}

int otLog::getLogLevel() {
	return this->loglevel;
}

void otLog::setLogLevel(int n) {
	this->loglevel = n;
}

std::string otLog::getLogLevelName(int n) {
	switch ( n ) {
		case CRITICAL:	return "Critical";
		case ERROR:		return "Error";
		case WARNING:	return "Warning";
		case INFO:		return "Info";
		case DEBUG:		return "Debug";
	}
	return "Unknown";
}
