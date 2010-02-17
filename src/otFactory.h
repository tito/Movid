#ifndef OT_FACTORY_H
#define OT_FACTORY_H

#include <vector>
#include <string>

#include "otModule.h"

class otFactory {
public:
	static otModule *create(const char *name);
	static std::vector<std::string> list();
};

#endif

