#ifndef MO_PIPELINE_H
#define MO_PIPELINE_H

#include <vector>
#include "moModule.h"

class moPipeline : public moModule {
public:

	moPipeline();
	virtual ~moPipeline();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);

	virtual void addElement(moModule *module);
	virtual void removeElement(moModule *module);

	virtual int getInputCount();
	virtual int getOutputCount();
	virtual moDataStreamInfo *getInputInfos(int n=0);
	virtual moDataStreamInfo *getOutputInfos(int n=0);

	virtual void start();
	virtual void stop();
	virtual void update();

	moModule *firstModule();
	moModule *lastModule();
	moModule *getModule(unsigned int index);
	unsigned int size();

	virtual void setGroup(bool group=true);
	virtual bool isGroup();
	virtual bool isPipeline();

private:
	std::vector<moModule *> modules;
	bool is_group;

	MODULE_INTERNALS();
};

#endif

