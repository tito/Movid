#ifndef MO_DAEMON_H
#define MO_DAEMON_H

class moDaemon {
public:
	static moDaemon *getInstance();

	void init();
	void cleanup();

private:
	moDaemon();
	virtual ~moDaemon();
};

#endif
