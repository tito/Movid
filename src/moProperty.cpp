/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "moProperty.h"
#include "moUtils.h"

#ifdef _WIN32
	#define snprintf _snprintf
#endif

#define CASTEDGET(x) x value = *(static_cast<x*>(val));

#define AUTOCONVERT(typein, value) \
	switch ( this->type ) { \
		case MO_PROPERTY_BOOL: \
			*(static_cast<bool*>(this->val)) = convertToBool(typein, &value); \
			break; \
		case MO_PROPERTY_STRING: \
			*(static_cast<std::string*>(this->val)) = convertToString(typein, &value); \
			break; \
		case MO_PROPERTY_INTEGER: \
			*(static_cast<int*>(this->val)) = convertToInteger(typein, &value); \
			break; \
		case MO_PROPERTY_DOUBLE: \
			*(static_cast<double*>(this->val)) = convertToDouble(typein, &value); \
			break; \
		case MO_PROPERTY_POINTLIST: \
			*(static_cast<moPointList*>(this->val)) = convertToPointList(typein, &value); \
			break; \
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

		case MO_PROPERTY_POINTLIST: {
			CASTEDGET(moPointList);
			return value.size() > 0;
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

		case MO_PROPERTY_POINTLIST: {
			std::ostringstream oss;
			moPointList::iterator it;
			CASTEDGET(moPointList);
			for ( it = value.begin(); it != value.end(); it++ )
				oss << (*it).x << "," << (*it).y << ";";
			return oss.str();
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

		case MO_PROPERTY_POINTLIST: {
			CASTEDGET(moPointList);
			return value.size();
		}

		default:;
		assert(0);
	}

	assert(0);
	return 0;
}

static moPointList convertToPointList(moPropertyType type, void *val) {
	moPointList output = moPointList();
	switch ( type ) {
		case MO_PROPERTY_STRING: {
			CASTEDGET(std::string);
			std::vector<std::string> points = moUtils::tokenize(value, ";");
			std::vector<std::string>::iterator it;
			for ( it = points.begin(); it != points.end(); it++ ) {
				std::vector<std::string> point = moUtils::tokenize((*it), ",");
				moPoint p;

				// it's an error, not 2 points. just forget it.
				if ( point.size() != 2 )
					continue;

				// push the point into the list
				p.x = atof(point[0].c_str());
				p.y = atof(point[1].c_str());
				output.push_back(p);
			}

			return output;
		}

		case MO_PROPERTY_POINTLIST: {
			CASTEDGET(moPointList);
			return value;
		}

		// we can't do anything for other type.
		default:
			return output;
	}

	return output;
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

moProperty::moProperty(moPointList value, const std::string &description) {
	this->init(description);
	this->type = MO_PROPERTY_POINTLIST;
	this->val = new moPointList();
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
	this->callbacks = std::map<moPropertyCallback,void*>();
	this->setDescription(description);
}

void moProperty::set(bool value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_BOOL, value);
	this->fireCallback();
}

void moProperty::set(std::string value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_STRING, value);
	this->fireCallback();
}

void moProperty::set(const char *value) {
	if ( this->isReadOnly() )
		return;
	std::string s = std::string(value);
	AUTOCONVERT(MO_PROPERTY_STRING, s);
	this->fireCallback();
}

void moProperty::set(int value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_INTEGER, value);
	this->fireCallback();
}

void moProperty::set(double value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_DOUBLE, value);
	this->fireCallback();
}

void moProperty::set(moPointList value) {
	if ( this->isReadOnly() )
		return;
	AUTOCONVERT(MO_PROPERTY_POINTLIST, value);
	this->fireCallback();
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

moPointList moProperty::asPointList() {
	return convertToPointList(this->type, this->val);
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
		case MO_PROPERTY_POINTLIST: return "pointlist";
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
		case MO_PROPERTY_POINTLIST: return o << f->asPointList();
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

void moProperty::addCallback(moPropertyCallback callback, void *userdata) {
	std::map<moPropertyCallback, void*>::iterator it;
	// ensure no callback already exist for this property
	assert( this->callbacks.find(callback) == this->callbacks.end() );
	this->callbacks[callback] = userdata;
}

void moProperty::removeCallback(moPropertyCallback callback) {
	std::map<moPropertyCallback, void*>::iterator it;
	for ( it = this->callbacks.begin(); it != this->callbacks.end(); it++ ) {
		if ( it->first == callback ) {
			this->callbacks.erase(it);
			return;
		}
	}
}

void moProperty::fireCallback() {
	std::map<moPropertyCallback, void*>::iterator it;
	for ( it = this->callbacks.begin(); it != this->callbacks.end(); it++ )
		it->first(this, it->second);
}

void moProperty::setText(bool is_text) {
	this->is_text = is_text;
}

bool moProperty::isText() {
	return this->is_text;
}

