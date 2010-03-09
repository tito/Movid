#ifndef MO_PROPERTY_H
#define MO_PROPERTY_H

#include <ostream>
#include <string>

typedef enum _moPropertyType {
	MO_PROPERTY_NONE,
	MO_PROPERTY_BOOL, 
	MO_PROPERTY_STRING, 
	MO_PROPERTY_INTEGER,
	MO_PROPERTY_DOUBLE
} moPropertyType;

class moProperty {
public:
	moProperty(bool value, const std::string &description = "");
	moProperty(const char* value, const std::string &description = "");
	moProperty(std::string value, const std::string &description = "");
	moProperty(int value, const std::string &description = "");
	moProperty(double value, const std::string &description = "");
	~moProperty();
	
	moPropertyType getType();

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

	bool haveMin();
	bool haveMax();
	bool haveChoices();
	int getMin();
	int getMax();
	std::string getChoices();
	void setMin(int val);
	void setMax(int val);
	void setChoices(const std::string &val);
	
	friend std::ostream& operator<< (std::ostream& o, const moProperty& f);

	static std::string getPropertyTypeName(moPropertyType type);
	
private:
	moProperty(const moProperty& property);
	moPropertyType type;
	std::string description;
	void* val;
	bool readonly;
	bool have_min;
	bool have_max;
	bool have_choices;
	int val_min;
	int val_max;
	std::string val_choices;
	
	void free();
	void init(const std::string& description);
};

#endif

