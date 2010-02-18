#ifndef OT_DATASTRUCT_H
#define OT_DATASTRUCT_H

#include <string>
#include <map>
#include <vector>

class otProperty;

class otDataGenericContainer {
public:
	otDataGenericContainer();
	virtual ~otDataGenericContainer();

	bool exist(const std::string &name);

	std::map<std::string, otProperty*> properties;
};

typedef std::vector<otDataGenericContainer *> otDataGenericList;

#endif

