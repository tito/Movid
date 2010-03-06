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

