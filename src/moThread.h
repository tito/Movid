#ifndef MO_THREAD_H
#define MO_THREAD_H

#include "pasync.h"

class moThread;

typedef void (*thread_process_t)(moThread *thread);

class moThread : public pt::thread {
public:
	moThread(thread_process_t _process, void *_userdata);
	virtual ~moThread();
	void *getUserData();
	void execute();
	void stop();
	bool wantQuit();

private:
	thread_process_t process;
	void *userdata;
	bool want_quit;
};

#endif
