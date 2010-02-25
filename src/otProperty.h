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
	otProperty(bool value, const std::string &description = "");
	otProperty(const char* value, const std::string &description = "");
	otProperty(std::string value, const std::string &description = "");
	otProperty(int value, const std::string &description = "");
	otProperty(double value, const std::string &description = "");
	~otProperty();
	
	otPropertyType getType();

	void setDescription(const std::string& desc);
	std::string getDescription();

	void setReadOnly(bool);
	bool isReadOnly();
	
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
	std::string description;
	void* val;
	bool readonly;
	
	void free();
	void init(const std::string& description);
};

#endif

