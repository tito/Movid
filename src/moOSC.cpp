#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef WIN32
// TODO
#else
#ifdef LINUX
// TODO
#else
// OTHERS
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif // ifdef LINUX
#endif // ifdef WIN32

#include "moOSC.h"

#include "moFactory.h"

moOSC::moOSC(const std::string &ip, unsigned short port) {
	this->ip = ip;
	this->port = port;
	this->sock = -1;
}

moOSC::~moOSC() {
}

void moOSC::init() {
	this->sock = socket(AF_INET, SOCK_DGRAM, 0);
}

void moOSC::send(WOscMessage *msg) {
	struct sockaddr_in servaddr = {0};

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
	servaddr.sin_port = htons(this->port);

	sendto(this->sock, msg->GetBuffer(), msg->GetBufferLen(), 0,
		 (struct sockaddr *)&servaddr, sizeof(servaddr));
}

