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
	bool relax(int);

private:
	thread_process_t process;
	void *userdata;
	bool want_quit;
};

#endif
