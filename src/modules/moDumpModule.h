#ifndef MO_DUMP_MODULE_H
#define MO_DUMP_MODULE_H

#include "../moModule.h"

class moDataStream;

class moDumpModule : public moModule {
public:
	moDumpModule(); 
	virtual ~moDumpModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);
	virtual void notifyData(moDataStream *stream);
	void update();

private:
	moDataStream *stream;
	MODULE_INTERNALS();
};


#endif

