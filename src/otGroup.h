#ifndef OT_GROUP_H
#define OT_GROUP_H

#include <list>
#include "otModule.h"

class otGroup : public otModule {
public:

	otGroup();
	virtual ~otGroup();

	void addElement(otModule *module);
	void removeElement(otModule *module);

	void setInput(otDataStream* , int n);
	otDataStream* getOutput(int n);

private:
	std::list<otModule *> modules;
};

#endif
