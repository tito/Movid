#ifndef OT_DAEMON_H
#define OT_DAEMON_H

class otDaemon {
public:
	static otDaemon *getInstance();

	void init();
	void cleanup();

private:
	otDaemon();
	virtual ~otDaemon();
};

#endif
