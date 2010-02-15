#ifndef OT_LOG_H
#define OT_LOG_H

#include <sstream>
#include <iostream>
#include <string>

#define LOG_DECLARE(x) \
	static char log_name[] = x;

#define LOG(x) otLogMessage(log_name, __FILE__, __LINE__, x).get()

enum {
	CRITICAL	= 0,
	ERROR		= 1,
	WARNING		= 2,
	INFO		= 3,
	DEBUG		= 4,
};

class otLogMessage {
public:
	otLogMessage(std::string name, std::string filename, int line, int level);
	~otLogMessage();

	std::ostringstream &get();

private:
	std::ostringstream os;
	int level;
};

class otLog {
public:
	static otLog *getInstance();
	void setLogLevel(int n);
	int getLogLevel();
	std::string getLogLevelName(int n);

private:
	otLog();
	~otLog();
	int loglevel;
};

#endif

