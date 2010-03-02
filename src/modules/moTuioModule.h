#ifndef MO_TUIO_MODULE_H
#define MO_TUIO_MODULE_H

#include <string>
#include "../moModule.h"
#include "../moOSC.h"

class moDataStream;

class moTuioModule : public moModule {
public:
	moTuioModule();
	virtual ~moTuioModule();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);

	void notifyData(moDataStream *stream);
	void update();

	void start();
	void stop();

private:
	moDataStream *input;
	moOSC *osc;
	int fseq;

	MODULE_INTERNALS();
};

#endif


