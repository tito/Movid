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



//
// Basic abstraction of thread/semaphore for WIN32 and Posix
//

#include "moThread.h"

#include "pasync.h"

moThread::moThread(thread_process_t _process, void *_userdata) : pt::thread(false) {
	this->process = _process;
	this->userdata = _userdata;
	this->want_quit = false;
}


moThread::~moThread() {
}

void moThread::execute() {
	this->process(this);
}

void *moThread::getUserData() {
	return this->userdata;
}

void moThread::stop() {
	this->want_quit = true;
}

bool moThread::wantQuit() {
	return this->want_quit;
}

bool moThread::relax(int m) {
	return pt::thread::relax(m);
}
