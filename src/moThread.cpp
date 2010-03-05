//
// Basic abstraction of thread/semaphore for WIN32 and Posix
//

#include "moThread.h"

thread_id_t get_current_thread_id() {
#ifdef WIN32
    return (int)GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

moThread::moThread(thread_process_t _process, void *_userdata) {
#ifdef WIN32
    this->id = 0;
#endif
	this->finished = 0;
	this->handle = 0;
	this->process = _process;
	this->userdata = _userdata;
	this->want_quit = false;
}


moThread::~moThread() {
#ifdef WIN32
	CloseHandle(handle);
#else
	pthread_detach(handle);
#endif
}

thread_id_t moThread::id() {
#ifdef WIN32
    return id;
#else
    return this->handle;
#endif
}


void moThread::wait() {
	if ( this->id() != get_current_thread_id() ) {
		// FIXME
		return;
	}
	this->want_quit = true;
#ifdef WIN32
    WaitForSingleObject(this->handle, INFINITE);
    CloseHandle(this->handle);
#else
    pthread_join(this->handle, 0);
#endif
    this->handle = 0;
}


#ifdef WIN32
unsigned _stdcall _thread_process(void* arg)
{
#else
void* _thread_process(void* arg)
{
#endif
    moThread* thread = (moThread*)arg;
#ifndef WIN32
	thread->setHandle(pthread_self());
#endif

	thread->run();
	thread->cleanup();
    return 0;
}

void moThread::run() {
	this->process(this);
}

bool moThread::start() {
#ifdef WIN32
	this->handle = (HANDLE)_beginthreadex(0, 0, _thread_process, this, 0, &this->id);
	if ( this->handle == 0 )
		return false;
#else
	size_t sz;
	int ret;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	if ( pthread_attr_getstacksize(&attr, &sz) == 0 && sz < THREAD_STACK_SIZE )
		pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&this->handle, &attr, _thread_process, this);
	pthread_attr_destroy(&attr);
	if ( ret != 0 )
		return false;
#endif
	return true;
}

void moThread::cleanup() {
	this->wait();
}

void moThread::setHandle(pthread_t _handle) {
	this->handle = _handle;
}

bool moThread::wantQuit() {
	return this->want_quit;
}

void *moThread::getUserData() {
	return this->userdata;
}
