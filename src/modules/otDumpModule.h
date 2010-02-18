#ifndef OT_DUMP_MODULE_H
#define OT_DUMP_MODULE_H

#include "../otModule.h"

class otDataStream;

class otDumpModule : public otModule {
public:
	otDumpModule(); 
	virtual ~otDumpModule();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);
	virtual void notifyData(otDataStream *stream);
	void update();

private:
	otDataStream *stream;
	MODULE_INTERNALS();
};


#endif

