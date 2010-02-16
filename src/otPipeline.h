#ifndef OT_PIPELINE_H
#define OT_PIPELINE_H

#include <vector>
#include "otModule.h"

class otPipeline : public otModule {
public:

	otPipeline();
	virtual ~otPipeline();

	virtual void addElement(otModule *module);
	virtual void removeElement(otModule *module);

	virtual void setInput(otDataStream* , int n);
	virtual otDataStream* getOutput(int n);

	virtual int getInputCount();
	virtual int getOutputCount();
	virtual std::string getInputName(int n=0);
	virtual std::string getOutputName(int n=0);
	virtual std::string getInputType(int n=0);
	virtual std::string getOutputType(int n=0);

	virtual void start();
	virtual void stop();
	virtual void update();
private:
	std::vector<otModule *> modules;
};

#endif

