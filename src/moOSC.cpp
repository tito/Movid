#include <iostream>
#include <stdio.h>

#ifdef _WIN32
	#include <WinSock2.h>
	#include <ws2ipdef.h>
	#define ssize_t SSIZE_T
	#define close(s) shutdown(s, SD_BOTH)
#else // OTHERS
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <arpa/inet.h>
#endif // ifdef WIN32

#include "moOSC.h"
#include "moLog.h"
#include "moFactory.h"

LOG_DECLARE("OSC");

moOSC::moOSC(const std::string &ip, unsigned short port) {
	this->ip = ip;
	this->port = port;
	this->sock = -1;
	this->init();
}

moOSC::~moOSC() {
	if ( this->sock >= 0 )
		close(this->sock);
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

void moOSC::send(WOscBundle *msg) {
	struct sockaddr_in servaddr = {0};
	ssize_t ret;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
	servaddr.sin_port = htons(this->port);

	ret = sendto(this->sock, msg->GetBuffer(), msg->GetBufferLen(), 0,
		 (struct sockaddr *)&servaddr, sizeof(servaddr));

	LOG(TRACE) << "send " << ret << " vs " << msg->GetBufferLen();
}

