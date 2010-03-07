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
	this->waitfor();
}

bool moThread::wantQuit() {
	return this->want_quit;
}

bool moThread::relax(int m) {
	return pt::thread::relax(m);
}
