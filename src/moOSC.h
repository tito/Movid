#ifndef MO_OSC_H
#define MO_OSC_H

#include "WOscMessage.h"
#include <iostream>

class moOSC {
public:
	moOSC(const std::string &ip, unsigned short port);
	virtual ~moOSC();
	void send(WOscMessage *msg);

private:
	void init();

	int sock;
	std::string ip;
	unsigned short port;
};

#endif

