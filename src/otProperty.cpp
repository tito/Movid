#include "otProperty.h"

otProperty::otProperty(){
	this->type = OT_PROPERTY_BOOL;
	this->val = new bool(val);
}

otProperty::~otProperty(){
	this->free_value();
}

otPropertyType otProperty::getType(){
	return	this->type;
}

bool otProperty::getBoolValue(){
	//TODO convert from str or float to bool
	if (this->type == OT_PROPERTY_BOOL)
		return *(bool*)(this->val);
}

const char* otProperty::getStringValue(){
	if (this->type == OT_PROPERTY_STRING)
		return (const char*)this->val;
	if (this->type == OT_PROPERTY_BOOL)
		return *(float*)(this->val) ? "True" : "False";
}

float otProperty::getFloatValue(){
	//TODO covert from str to float
	if (this->type == OT_PROPERTY_FLOAT)
		return *(float*)(this->val);
	if (this->type == OT_PROPERTY_BOOL)
		return *(bool*)(this->val) ? 1.0 : 0.0;
	if (this->type == OT_PROPERTY_STRING)
		return atof((const char*)(this->val));
	
}

void otProperty::setValue(bool val){
	this->free_value();
	this->type = OT_PROPERTY_BOOL;
	this->val = new bool(val);
}

void otProperty::setValue(const char* val){
	this->free_value();
	this->type = OT_PROPERTY_STRING;
	this->val = (void*)val;
}

void otProperty::setValue(float val){
	this->free_value();
	this->type = OT_PROPERTY_FLOAT;
	this->val = new float(val);
}


void otProperty::free_value(){
	if (this->type != OT_PROPERTY_BOOL)
		delete (bool*)this->val;
	if (this->type != OT_PROPERTY_FLOAT)
		delete (float*)this->val;
}

