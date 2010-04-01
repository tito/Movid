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

	LOG(MO_TRACE) << "send " << ret << " vs " << msg->GetBufferLen();
}

