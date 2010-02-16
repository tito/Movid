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
#include <assert.h>
#include <ctype.h>

#include "otParser.h"
#include "otModule.h"
#include "otPipeline.h"
#include "otFactory.h"
#include "otLog.h"

#define INVALID_SYNTAX(error) do { \
		std::cerr << "otParser: invalid syntax: " << error << std::endl; \
		goto parse_error; \
	} while (0);


LOG_DECLARE("Parser");

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

static otModule *parseObject(std::string &str) {
	otModule *object = NULL;
	unsigned int index = 0;

	while ( index < str.size() && isalpha(str[index]) ) {
		index++;
	}

	if ( index >= str.size() ) {
		LOG(CRITICAL) << "invalid syntax, eof in object name";
		return NULL;
	}

	LOG(INFO) << "Create object <" << str.substr(0, index) << ">";

	object = otFactory::create(str.substr(0, index).c_str());
	if ( object == NULL )
	{
		LOG(CRITICAL) << "Object <" << str.substr(0, index) << "> don't exist";
		return NULL;
	}

	str.erase(0, index);

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
		goto parse_error;

	// Start of the ->, recurse if possible !
	if ( str[0] == '-' ) {
		if ( !parserCheckChar(str, '-') )
			INVALID_SYNTAX("'-' expected");
		if ( !parserCheckChar(str, '>') )
			INVALID_SYNTAX("'>' expected");

		next = parsePipelineContent(str, pipeline);

		// connect object together
		if ( next->getInputCount() != object->getOutputCount() ) {
			LOG(CRITICAL) << "Input / output don't have the same number of entries";
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
		goto parse_error;

	if ( !parseGroupContent(str, group) )
		goto parse_error;

	if ( !parserSkipSpaces(str) )
		goto parse_error;

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
