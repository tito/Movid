#ifndef OT_FACTORY_H
#define OT_FACTORY_H

#include <vector>
#include <map>
#include <string>

#include "otModule.h"

typedef otModule *(*otFactoryCreateCallback)();

class otFactory {
public:
	~otFactory();

	static otFactory *getInstance();
	static void init();

	void registerModule(const std::string &name, otFactoryCreateCallback callback);

	otModule *create(const std::string &name);
	std::vector<std::string> list();

protected:
	otFactory();

	std::map<std::string, otFactoryCreateCallback> database;
};

#endif

