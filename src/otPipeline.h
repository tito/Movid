#ifndef OT_PIPELINE_H
#define OT_PIPELINE_H

#include <vector>
#include "otModule.h"

class otPipeline : public otModule {
public:

	otPipeline();
	virtual ~otPipeline();

	virtual void setInput(otDataStream* stream, int n=0);
	virtual otDataStream *getInput(int n=0);
	virtual otDataStream *getOutput(int n=0);

	virtual void addElement(otModule *module);
	virtual void removeElement(otModule *module);

	virtual int getInputCount();
	virtual int getOutputCount();
	virtual otDataStreamInfo *getInputInfos(int n=0);
	virtual otDataStreamInfo *getOutputInfos(int n=0);

	virtual void start();
	virtual void stop();
	virtual void update();

	otModule *firstModule();
	otModule *lastModule();
	otModule *getModule(unsigned int index);
	unsigned int size();

	virtual void setGroup(bool group=true);
	virtual bool isGroup();
	virtual bool isPipeline();

private:
	std::vector<otModule *> modules;
	bool is_group;

	MODULE_INTERNALS();
};

#endif

