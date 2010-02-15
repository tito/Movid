#ifndef OT_PROPERTY_H
#define OT_PROPERTY_H


enum otPropertyType {OT_PROPERTY_BOOL, OT_PROPERTY_STRING, OT_PROPERTY_FLOAT}

class otProperty {
public:
	
	otProperty();
	~otProperty();
	
	otPropertyType getType();
	
	bool getBoolValue();
	const char* getStringValue();
	float getFloatValue();
	
	void setValue(bool val);
	void setValue(const char* val);
	void setValue(float val);

	
private:
	otPropertyType type;
	void* val;
	
	void free_value();
};

#endif

