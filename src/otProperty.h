#ifndef OT_PROPERTY_H
#define OT_PROPERTY_H

#include <ostream>
#include <string>

typedef enum _otPropertyType {
	OT_PROPERTY_NONE,
	OT_PROPERTY_BOOL, 
	OT_PROPERTY_STRING, 
	OT_PROPERTY_INTEGER,
	OT_PROPERTY_DOUBLE
} otPropertyType;

class otProperty {
public:
	otProperty(bool value);
	otProperty(const char* value);
	otProperty(std::string value);
	otProperty(int value);
	otProperty(double value);
	~otProperty();
	
	otPropertyType getType();
	
	bool asBool();
	std::string asString();
	double asDouble();
	int asInteger();

	void set(bool value);
	void set(const char* value);
	void set(std::string value);
	void set(int value);
	void set(double value);
	
	friend std::ostream& operator<< (std::ostream& o, const otProperty& f);

	static std::string getPropertyTypeName(otPropertyType type);
	
private:
	otProperty(const otProperty& property);
	otPropertyType type;
	void* val;
	
	void free();
};

#endif

