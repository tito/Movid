//
// Pipeline Notation :
// GROUP -> '{' PIPELINE '}'
// PIPELINE -> GROUP | OBJECT ['->' PIPELINE]
// OBJECT -> OBJECT_NAME [OBJECT_PROPERTIES]
// OBJECT_NAME -> ot[A-Z][A-Za-z0-9]*
// OBJECT_PROPERTIES -> '(' PROPERTIES ')'
// PROPERTIES -> [PROPERTIES ',']PROPERTY
// PROPERTY -> [a-z][A-Za-z0-9]* '=' VALUE
// VALUE -> STRING | NUMERIC | BOOL
// STRING -> '"' [.*]* '"'
// NUMERIC -> [0-9]*[.[0-9]*]
// BOOL -> 0 | 1
//
// Examples :
// {{otCamera(index=0) → otBlobTracker → otTuioConverter}, {otCamera(index=1) → otBlobTracker → otTuioConverter}} → otTuioMultiplexer → otTuioTCPSender(port=3333)
// otCamera(index=0) → otSplitter → {otBlobTracker → otTuioConverter → otTuioTCPSender(port=3333), otHandTracker → otTuioConverter → otTuioTCPSender(port=3334)}
//

#include <iostream>
#include <cstdio>
#include <string.h>
#include <sstream>
#include <assert.h>
#include <ctype.h>

#include "otParser.h"
#include "otModule.h"
#include "otPipeline.h"
#include "otFactory.h"
#include "otLog.h"

#define INVALID_SYNTAX(error) do { \
		LOG(CRITICAL) << "invalid syntax: " << error << std::endl; \
		goto parse_error; \
	} while (0);


LOG_DECLARE("Parser");

//used to convert string to other type.  example :
// bool b; string value = "true";
// from_string<bool>(b, value, std::boolalpha)  -> returns true

template <class T>
bool from_string(T &t,
                 const std::string &s,
                 std::ios_base & (*f)(std::ios_base&))
{
    std::istringstream iss(s);
    return !(iss>>f>>t).fail();
}

// Skip all spaces/tabs/lines availables
static bool parserSkipSpaces(std::string &str) {
	while ( str.size() && isspace(str[0]) )
		str.erase(str.begin());

	return str.size() ? true : false;
}

// Ensure that a char is the one wanted, if yes, eat it.
static bool parserCheckChar(std::string &str, char wanted) {
	if ( str[0] == wanted ) {
		str.erase(str.begin());
		return true;
	}

	return false;
}

static bool parseNumericToken(std::string &str, std::string &token, bool strip_space=true) {
	unsigned int index = 0;
	bool have_dot = false;

	if ( strip_space && !parserSkipSpaces(str) )
		return false;

	while ( index < str.size() && (isdigit(str[index]) || (!have_dot && str[index] == '.')) ) {
		if ( str[index] == '.' )
			have_dot = true;
		index++;
	}

	if ( index >= str.size() )
		return false;

	token = str.substr(0,index);
	str.erase(0, index);

	if ( strip_space && !parserSkipSpaces(str) )
		return false;

	return true;

}

static bool parseStringToken(std::string &str, std::string &token, bool strip_space=true) {
	unsigned int index = 0;
	bool have_quote = false;
	char quote = '\0';

	if ( strip_space && !parserSkipSpaces(str) )
		return false;

	if ( str[index] == '\'' || str[index] == '\"') {
		have_quote = true;
		quote = str[index];
		str.erase(str.begin());
	}

	while ( index < str.size() ) {

		if ( str[index] == '\\' && have_quote ) {
			// peek the next char
			if ( index+1 < str.size() ) {
				if ( str[index+1] == quote || str[index+1] == '\\' ) {
					// remove current
					str.erase(index, 1);
					continue;
				}
			}
		}

		if ( have_quote && str[index] == quote )
			break;

		if ( !have_quote && !isalpha(str[index]) )
			break;

		index++;
	}

	if ( index >= str.size() )
		return false;

	token = str.substr(0, index);
	str.erase(0, index + (have_quote ? 1 : 0));

	if ( strip_space && !parserSkipSpaces(str) )
		return false;

	return true;

}

static bool parseObjectProperty(std::string &str, otModule* object) {
	std::string name;
	std::string value;
	int i; bool b; double d;


	if ( !parseStringToken(str, name) ) {
		LOG(CRITICAL) << "invalid syntax, eof in property name";
		return false;
	}

	if ( !parserCheckChar(str, '=') ) {
		LOG(CRITICAL) << "invalid syntax, expected '=' after property name: " << name ;
		return false;
	}

	if ( !parseStringToken(str, value) ) {
		LOG(CRITICAL) << "invalid syntax, eof in property value";
		return false;
	}

	if ( value.size() ) {
		// check to see if its boolean ('true' or false)
		if (from_string<bool>(b, value, std::boolalpha)) {
			object->property(name).set(b);
			LOG(TRACE) << "set Property <" << name << "> to " << b << "(bool)";
			return true;
		}

		// otherwise set string value
		object->property(name).set(value);
		LOG(TRACE) << "set Property <" << name << "> to " << value << "(string)";
		return true;
	}

	// wasnt a string, so parse a number
	if ( !parseNumericToken(str, value) ) {
		LOG(CRITICAL) << "invalid syntax, eof in property value";
		return false;
	}

	// check to see if its int
	if ( from_string<int>(i, value, std::dec) ) {
		object->property(name).set(i);
		LOG(TRACE) << "set Property <" << name << "> to " << i << "(int)";
		return true;
	}

	// check to see if its float
	if ( from_string<double>(d, value, std::dec) ) {
		object->property(name).set(d);
		LOG(TRACE) << "set Property <" << name << "> to " << d << "(double)";
		return true;
	}

	INVALID_SYNTAX("Unable to parse value");

parse_error:;
	return false;
}

static bool parseObjectPropertyList(std::string &str, otModule* object) {
	if ( !parserSkipSpaces(str) )
		return false;

	// no properties to parse...so justy return happily
	if ( !parserCheckChar(str, '(') )
		return true;

	// parse actual property values as long as they are , seperated
	if ( !parseObjectProperty(str, object) )
		return false;

	while ( parserCheckChar(str,',') ) {
		if ( !parseObjectProperty(str, object) )
			return false;
	}

	// have to have clsing bracket if we got this far
	if ( !parserCheckChar(str, ')') )
		return false;

	return true;
}

static otModule *parseObject(std::string &str) {
	otModule *object = NULL;
	std::string obj_name;

	if (!parseStringToken(str, obj_name)) {
		LOG(CRITICAL) << "invalid syntax, eof in object name";
		return false;
	}

	LOG(TRACE) << "create object <" << obj_name << ">";
	object = otFactory::create(obj_name.c_str());
	if ( object == NULL )
	{
		LOG(CRITICAL) << "object <" << obj_name << "> don't exist";
		return NULL;
	}

	if ( !parseObjectPropertyList(str, object) )
		return NULL;

	return object;
}

// Parse a pipeline content (an object and maybe -> + recurse)
static otPipeline *parsePipeline(std::string &str);
static otModule *parsePipelineContent(std::string &str, otPipeline* pipeline) {
	otModule *object = NULL,
			 *next;

	if ( !parserSkipSpaces(str) )
		return NULL;

	// Parse a object
	object = parseObject(str);
	pipeline->addElement(object);

	if ( !parserSkipSpaces(str) )
		INVALID_SYNTAX("premature end of pipeline description while parsing pipeline");

	// Start of the ->, recurse if possible !
	if ( str[0] == '-' ) {
		if ( !parserCheckChar(str, '-') )
			INVALID_SYNTAX("'-' expected");
		if ( !parserCheckChar(str, '>') )
			INVALID_SYNTAX("'>' expected");

		next = parsePipelineContent(str, pipeline);

		// connect object together
		if ( next->getInputCount() != object->getOutputCount() ) {
			LOG(CRITICAL) << "input / output don't have the same number of entries";
			goto parse_error;
		}

		for ( int i = 0; i < next->getInputCount(); i++ )
			next->setInput(object->getOutput(i), i);

		return object;
	}

	return object;

parse_error:;
	if ( object != NULL ) {
		pipeline->removeElement(object);
		delete object;
	}
	return NULL;
}

// Parse a pipeline (object -> object -> object)
static otPipeline *parsePipeline(std::string &str) {
	otPipeline *pipeline;

	pipeline = new otPipeline();

	if ( parsePipelineContent(str, pipeline) == NULL )
	{
		delete pipeline;
		return NULL;
	}

	return pipeline;
}

// Parse the content of a group
static otPipeline *parseGroup(std::string &str);
static bool parseGroupContent(std::string &str, otPipeline* group) {
	otPipeline *child;

	// Check if it's a new group, or a pipeline
	if ( str[0] == '{' )
		child = parseGroup(str);
	else
		child = parsePipeline(str);

	group->addElement(child);

	if ( !parserSkipSpaces(str) )
		return false;

	// Recursive if we got a ,
	if ( parserCheckChar(str, ',') )
		return parseGroupContent(str, group);

	return true;
}

// Parse a group { ... }
static otPipeline *parseGroup(std::string &str) {
	otPipeline *group = NULL;

	if ( !parserSkipSpaces(str) )
		return NULL;

	if ( !parserCheckChar(str, '{') )
		INVALID_SYNTAX("'{' expected");

	group = new otPipeline();

	if ( !parserSkipSpaces(str) )
		INVALID_SYNTAX("premature end of pipeline description while parsing group");

	if ( !parseGroupContent(str, group) )
		goto parse_error;

	if ( !parserSkipSpaces(str) )
		INVALID_SYNTAX("premature end of pipeline description while parsing group");

	if ( !parserCheckChar(str, '}') )
		INVALID_SYNTAX("'}' expected");

	return group;

parse_error:;
	if ( group == NULL )
		delete group;
	return NULL;
}


//
// Public functions
//

otPipeline* otParser::parseString(std::string str) {
	return parseGroup(str);
}
