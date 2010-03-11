#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "moProperty.h"

#ifdef _WIN32
	#define snprintf _snprintf
#endif

#define CASTEDGET(x) x value = *(static_cast<x*>(val));

#define AUTOCONVERT(typein, value) \
	switch ( this->type ) { \
		case MO_PROPERTY_BOOL: \
			*(static_cast<bool*>(this->val)) = convertToBool(typein, &value); \
			return; \
		case MO_PROPERTY_STRING: \
			*(static_cast<std::string*>(this->val)) = convertToString(typein, &value); \
			return; \
		case MO_PROPERTY_INTEGER: \
			*(static_cast<int*>(this->val)) = convertToInteger(typein, &value); \
			return; \
		case MO_PROPERTY_DOUBLE: \
			*(static_cast<double*>(this->val)) = convertToDouble(typein, &value); \
			return; \
		default:; \
	}


static bool convertToBool(moPropertyType type, void *val) {
	switch ( type ) {
		case MO_PROPERTY_BOOL: {
			CASTEDGET(bool);
			return value;
		}

		case MO_PROPERTY_STRING: {
			CASTEDGET(std::string);
			if ( value == "True" || value == "true" || value == "TRUE" || value == "1" )
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

		default:
		assert(0);
	}

	assert(0);
	return false;
}

static std::string convertToString(moPropertyType type, void *val) {
	switch ( type ) {
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
		assert(0);
	}

	assert(0);
	return "";
}

static double convertToDouble(moPropertyType type, void *val) {
	switch ( type ) {
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
		assert(0);
	}

	assert(0);
	return 0.0;
}

static int convertToInteger(moPropertyType type, void *val) {
	switch ( type ) {
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
		assert(0);
	}

	assert(0);
	return 0;
}

moProperty::moProperty(bool value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_BOOL;
	this->val = new bool();
	this->set(value);
}

moProperty::moProperty(const char *value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_STRING;
	this->val = new std::string();
	this->set(value);
}

moProperty::moProperty(std::string value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_STRING;
	this->val = new std::string();
	this->set(value);
}

moProperty::moProperty(int value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_INTEGER;
	this->val = new int();
	this->set(value);
}

moProperty::moProperty(double value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_DOUBLE;
	this->val = new double();
	this->set(value);
}

void moProperty::init(const std::string &description) {
	this->readonly = false;
	this->val = NULL;
	this->val_min = 0;
	this->val_max = 0;
	this->val_choices = "";
	this->have_min = false;
	this->have_max = false;
	this->have_choices = false;
	this->setDescription(description);
}

void moProperty::set(bool value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_BOOL, value);
}

void moProperty::set(std::string value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_STRING, value);
}

void moProperty::set(const char *value) {
	if ( this->isReadOnly() )
		return;
	std::string s = std::string(value);
	AUTOCONVERT(MO_PROPERTY_STRING, s);
}

void moProperty::set(int value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_INTEGER, value);
}

void moProperty::set(double value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_DOUBLE, value);
}

moProperty::~moProperty() {
	this->free();
}

moPropertyType moProperty::getType() {
	return this->type;
}

bool moProperty::asBool() {
	return convertToBool(this->type, this->val);
}

std::string moProperty::asString() {
	return convertToString(this->type, this->val);
}

double moProperty::asDouble() {
	return convertToDouble(this->type, this->val);
}

int moProperty::asInteger() {
	return convertToInteger(this->type, this->val);
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

bool moProperty::haveMin() {
	return this->have_min;
}

bool moProperty::haveMax() {
	return this->have_max;
}

bool moProperty::haveChoices() {
	return this->have_choices;
}

int moProperty::getMin() {
	return this->val_min;
}

int moProperty::getMax() {
	return this->val_max;
}

std::string moProperty::getChoices() {
	return this->val_choices;
}

void moProperty::setMin(int val) {
	this->val_min = val;
	this->have_min = true;
}

void moProperty::setMax(int val) {
	this->val_max = val;
	this->have_max = true;
}

void moProperty::setChoices(const std::string &val) {
	this->val_choices = val;
	this->have_choices = true;
}

