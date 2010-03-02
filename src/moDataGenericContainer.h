#ifndef MO_DATASTRUCT_H
#define MO_DATASTRUCT_H

#include <string>
#include <map>
#include <vector>

class moProperty;

class moDataGenericContainer {
public:
	moDataGenericContainer();
	virtual ~moDataGenericContainer();

	bool exist(const std::string &name);

	std::map<std::string, moProperty*> properties;
};

typedef std::vector<moDataGenericContainer *> moDataGenericList;

#endif

