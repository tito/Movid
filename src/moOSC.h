#ifndef MO_OSC_H
#define MO_OSC_H

#include "WOscBundle.h"
#include "WOscMessage.h"
#include <iostream>

class moOSC {
public:
	moOSC(const std::string &ip, unsigned short port);
	virtual ~moOSC();
	void send(WOscMessage *msg);
	void send(WOscBundle *msg);

private:
	void init();

	int sock;
	std::string ip;
	unsigned short port;
};

#endif

