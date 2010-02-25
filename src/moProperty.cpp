#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "moProperty.h"

#define CASTEDGET(x) x value = *(static_cast<x*>(this->val));

moProperty::moProperty(bool value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_NONE;
	this->set(value);
}

moProperty::moProperty(const char* value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_NONE;
	this->set(value);
}

moProperty::moProperty(std::string value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_NONE;
	this->set(value);
}

moProperty::moProperty(int value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_NONE;
	this->set(value);
}

moProperty::moProperty(double value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_NONE;
	this->set(value);
}

void moProperty::init(const std::string &description) {
	this->readonly = false;
	this->val = NULL;
	this->setDescription(description);
}

void moProperty::set(bool value) {
	if ( this->isReadOnly() )
		return;
	this->free();
	this->type = MO_PROPERTY_BOOL;
	this->val = new bool(value);
	this->setDescription(description);
}

void moProperty::set(const char* value) {
	if ( this->isReadOnly() )
		return;
	this->free();
	this->type = MO_PROPERTY_STRING;
	this->val = new std::string(value);
	this->setDescription(description);
}

void moProperty::set(std::string value) {
	if ( this->isReadOnly() )
		return;
	this->free();
	this->type = MO_PROPERTY_STRING;
	this->val = new std::string(value);
	this->setDescription(description);
}

void moProperty::set(int value) {
	if ( this->isReadOnly() )
		return;
	this->free();
	this->type = MO_PROPERTY_INTEGER;
	this->val = new int(value);
	this->setDescription(description);
}

void moProperty::set(double value) {
	if ( this->isReadOnly() )
		return;
	this->free();
	this->type = MO_PROPERTY_DOUBLE;
	this->val = new double(value);
	this->setDescription(description);
}

moProperty::~moProperty() {
	this->free();
}

moPropertyType moProperty::getType() {
	return this->type;
}

bool moProperty::asBool() {
	switch ( this->type ) {
		case MO_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value;
		}

		case MO_PROPERTY_STRING: {
			CASTEDGET(std::string);
			if ( value == "True" || value == "true" || value == "TRUE" )
				return true;
			return false;
		}

		case MO_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value == 0 ? false : true;
		}

		case MO_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value == 0 ? false : true;
		}

		default:;
	}
	return false;
}

std::string moProperty::asString() {
	switch ( this->type ) {
		case MO_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return value;
		}

		case MO_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? "true" : "false";
		}

		case MO_PROPERTY_INTEGER: {
			char buffer[64];
			CASTEDGET(int);
			snprintf(buffer, sizeof(buffer), "%d", value);
			return buffer;
		}

		case MO_PROPERTY_DOUBLE: {
			char buffer[64];
			CASTEDGET(double);
			snprintf(buffer, sizeof(buffer), "%f", value);
			return buffer;
		}

		default:;
	}

	return "";
}

double moProperty::asDouble() {
	switch ( this->type ) {
		case MO_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atof(value.c_str());
		}

		case MO_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1.0 : 0.0;
		}

		case MO_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return (double)value;
		}

		case MO_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return value;
		}

		default:;
	}

	return 0.0;
}

int moProperty::asInteger() {
	switch ( this->type ) {
		case MO_PROPERTY_STRING: {
			CASTEDGET(std::string);
			return atoi(value.c_str());
		}

		case MO_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value ? 1 : 0;
		}

		case MO_PROPERTY_INTEGER: {
			CASTEDGET(int);
			return value;
		}

		case MO_PROPERTY_DOUBLE: {
			CASTEDGET(double);
			return (int)value;
		}

		default:;
	}

	return 0;
}

void moProperty::free() {
	if ( this->val == NULL )
		return;

	switch ( this->type ) {
		case MO_PROPERTY_STRING:
			delete (std::string *)(this->val);
			break;
		case MO_PROPERTY_BOOL:
			delete static_cast<bool *>(this->val);
			break;
		case MO_PROPERTY_INTEGER:
			delete static_cast<int *>(this->val);
			break;
		case MO_PROPERTY_DOUBLE:
			delete static_cast<double *>(this->val);
			break;
		default:;
	}

	this->val = NULL;
}

std::string moProperty::getPropertyTypeName(moPropertyType type) {
	switch ( type ) {
		case MO_PROPERTY_DOUBLE: return "double";
		case MO_PROPERTY_INTEGER: return "integer";
		case MO_PROPERTY_STRING: return "string";
		case MO_PROPERTY_BOOL: return "bool";
		default:;
	}

	return "unknown";
}

std::ostream& operator<< (std::ostream& o, const moProperty& p) {

	// Bad bad ... :'(
	moProperty *f = (moProperty *)&p;

	switch ( f->getType() ) {
		case MO_PROPERTY_STRING:	return o << f->asString();
		case MO_PROPERTY_BOOL:		return o << f->asBool();
		case MO_PROPERTY_INTEGER:	return o << f->asInteger();
		case MO_PROPERTY_DOUBLE:	return o << f->asDouble();
		default:;
	}

	return o;
}

std::string moProperty::getDescription() {
	return this->description;
}

void moProperty::setDescription(const std::string& description) {
	this->description = description;
}

bool moProperty::isReadOnly() {
	return this->readonly;
}

void moProperty::setReadOnly(bool ro) {
	this->readonly = ro;
}
