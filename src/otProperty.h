#ifndef OT_PROPERTY_H
#define OT_PROPERTY_H

#include <ostream>
#include <string>

typedef enum _otPropertyType {
	OT_PROPERTY_BOOL, 
	OT_PROPERTY_STRING, 
	OT_PROPERTY_INTEGER,
	OT_PROPERTY_DOUBLE
} otPropertyType;

class otProperty {
public:
	otProperty(std::string name, bool value);
	otProperty(std::string name, const char* value);
	otProperty(std::string name, std::string value);
	otProperty(std::string name, int value);
	otProperty(std::string name, double value);
	~otProperty();
	
	otPropertyType getType();
	
	bool asBool();
	std::string asString();
	double asDouble();
	int asInteger();

	friend std::ostream& operator<< (std::ostream& o, const otProperty& f);
	
private:
	std::string name;
	otPropertyType type;
	void* val;
	
	void free();
};

#endif

