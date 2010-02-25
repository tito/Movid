#ifndef MO_FACTORY_H
#define MO_FACTORY_H

#include <vector>
#include <map>
#include <string>

#include "moModule.h"

typedef moModule *(*moFactoryCreateCallback)();

class moFactory {
public:
	~moFactory();

	static moFactory *getInstance();
	static void init();

	void registerModule(const std::string &name, moFactoryCreateCallback callback);

	moModule *create(const std::string &name);
	std::vector<std::string> list();

protected:
	moFactory();

	std::map<std::string, moFactoryCreateCallback> database;
};

#endif

