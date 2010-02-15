#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "otProperty.h"

#define CASTEDGET(x) x value = *(static_cast<x*>(this->val));

otProperty::otProperty() {
	this->val = NULL;
	this->type = OT_PROPERTY_NONE;
}

otProperty::otProperty(bool value) {
	otProperty();
	this->set(value);
}

otProperty::otProperty(const char* value) {
	otProperty();
	this->set(value);
}

otProperty::otProperty(std::string value) {
	otProperty();
	this->set(value);
}

otProperty::otProperty(int value) {
	otProperty();
	this->set(value);
}

otProperty::otProperty(double value) {
	otProperty();
	this->set(value);
}

void otProperty::set(bool value) {
	this->free();
	this->type = OT_PROPERTY_BOOL;
	this->val = new bool(value);
}

void otProperty::set(const char* value) {
	this->free();
	this->type = OT_PROPERTY_STRING;
	this->val = new std::string(value);
}

void otProperty::set(std::string value) {
	this->free();
	this->type = OT_PROPERTY_STRING;
	this->val = new std::string(value);
}

void otProperty::set(int value) {
	this->free();
	this->type = OT_PROPERTY_INTEGER;
	this->val = new int(value);
}

void otProperty::set(double value) {
	this->free();
	this->type = OT_PROPERTY_DOUBLE;
	this->val = new double(value);
}

otProperty::~otProperty() {
	this->free();
}

otPropertyType otProperty::getType() {
	return this->type;
}

bool otProperty::asBool() {
	switch ( this->type ) {
		case OT_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value;
		}

		case OT_PROPERTY_STRING: {
			CASTEDGET(std::string);
			if ( value == "True" || value == "true" || value == "TRUE" )
				return true;
			return false;
		}

		case OT_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value == 0 ? false : true;
		}

		case OT_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value == 0 ? false : true;
		}

		default:;
	}
	return false;
}

std::string otProperty::asString() {
	switch ( this->type ) {
		case OT_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return value;
		}

		case OT_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? "true" : "false";
		}

		case OT_PROPERTY_INTEGER: {
			char buffer[64];
			CASTEDGET(int);
			snprintf(buffer, sizeof(buffer), "%d", value);
			return buffer;
		}

		case OT_PROPERTY_DOUBLE: {
			char buffer[64];
			CASTEDGET(double);
			snprintf(buffer, sizeof(buffer), "%f", value);
			return buffer;
		}

		default:;
	}

	return "";
}

double otProperty::asDouble() {
	switch ( this->type ) {
		case OT_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atof(value.c_str());
		}

		case OT_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1.0 : 0.0;
		}

		case OT_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return (double)value;
		}

		case OT_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value;
		}

		default:;
	}

	return 0.0;
}

int otProperty::asInteger() {
	switch ( this->type ) {
		case OT_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atoi(value.c_str());
		}

		case OT_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1 : 0;
		}

		case OT_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value;
		}

		case OT_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return (int)value;
		}

		default:;
	}

	return 0;
}

void otProperty::free() {
	if ( this->val == NULL )
		return;

	switch ( this->type ) {
		case OT_PROPERTY_STRING:
			delete (std::string *)(this->val);
			break;
		case OT_PROPERTY_BOOL:
			delete static_cast<bool *>(this->val);
			break;
		case OT_PROPERTY_INTEGER:
			delete static_cast<int *>(this->val);
			break;
		case OT_PROPERTY_DOUBLE:
			delete static_cast<double *>(this->val);
			break;
		default:;
	}

	this->val = NULL;
}

std::ostream& operator<< (std::ostream& o, const otProperty& p) {

	// Bad bad ... :'(
	otProperty *f = (otProperty *)&p;

	switch ( f->getType() ) {
		case OT_PROPERTY_STRING:	return o << f->asString();
		case OT_PROPERTY_BOOL:		return o << f->asBool();
		case OT_PROPERTY_INTEGER:	return o << f->asInteger();
		case OT_PROPERTY_DOUBLE:	return o << f->asDouble();
		default:;
	}

	return o;
}

