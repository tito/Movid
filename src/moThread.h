#ifndef MO_THREAD_H
#define MO_THREAD_H

#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#endif

#define THREAD_STACK_SIZE 1024 * 1024

#ifdef WIN32
typedef unsigned thread_id_t;
#else
typedef pthread_t thread_id_t;
#endif

class moThread;

typedef void (*thread_process_t)(moThread *thread);

class moThread {
public:
	moThread(thread_process_t _process, void *_userdata);
	virtual ~moThread();
	void wait();
	bool start();
	void cleanup();
	void run();

	void *getUserData();
	bool wantQuit();

	thread_id_t	id();

	void setHandle(pthread_t _handle);

private:
#ifdef WIN32
	thread_id_t win_id;
#endif
	pthread_t handle;
	int finished;
	bool running;
	bool want_quit;
	thread_process_t process;
	void *userdata;
};

#endif
