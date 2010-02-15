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

#include "otParser.h"
#include "otModule.h"
#include "otGroup.h"

#define INVALID_SYNTAX(error) do { \
		std::cerr << "otParser: invalid syntax: " << error << std::endl; \
		goto parse_error; \
	} while (0);


// Return true if the char is a space/tabs/lines
static bool isSpace(char p) {
	return (p == ' ' || p == '\t' || p == '\r' || p == '\n') ? true : false;
}

// Skip all spaces/tabs/lines availables
static bool parserSkipSpaces(char **p, char *end) {
	while ( *p < end && isSpace(**p) )
		(*p)++;

	return *p < end ? true : false;
}

// Ensure that a char is the one wanted, if yes, eat it.
static bool parserCheckChar(char **p, char wanted) {
	if ( **p == wanted ) {
		(*p)++;
		return true;
	}
	return false;
}

static otModule *parseObject(char **p, char *end) {
	otModule *object;

	// FIXME: create the real object
	object = new otGroup();

	assert("unimplemented" && 0);

	return object;
}

// Parse a pipeline content (an object and maybe -> + recurse)
static otGroup *parsePipeline(char **p, char *end);
static bool parsePipelineContent(char **p, char *end, otGroup* pipeline) {
	otModule *object = NULL;

	if ( !parserSkipSpaces(p, end) )
		return false;

	// Parse a object
	object = parseObject(p, end);
	pipeline->addElement(object);

	if ( !parserSkipSpaces(p, end) )
		goto parse_error;

	// Start of the ->, recurse if possible !
	if ( **p == '-' ) {
		if ( !parserCheckChar(p, '-') )
			INVALID_SYNTAX("'-' expected");
		if ( !parserCheckChar(p, '>') )
			INVALID_SYNTAX("'>' expected");

		return parsePipelineContent(p, end, pipeline);
	}

	return true;

parse_error:;
	if ( object != NULL )
		delete object;
	if ( pipeline != NULL )
		delete pipeline;
	return false;
}

// Parse a pipeline (object -> object -> object)
static otGroup *parsePipeline(char **p, char *end) {
	otGroup *pipeline;

	// FIXME: Replace with real pipeline
	pipeline = new otGroup();

	if ( !parsePipelineContent(p, end, pipeline) )
	{
		delete pipeline;
		return NULL;
	}

	return pipeline;
}

// Parse the content of a group
static otGroup *parseGroup(char **p, char *end);
static bool parseGroupContent(char **p, char *end, otGroup* group) {
	otGroup *child;

	// Check if it's a new group, or a pipeline
	if ( **p == '{' )
		child = parseGroup(p, end);
	else
		child = parsePipeline(p, end);

	group->addElement(child);

	if ( !parserSkipSpaces(p, end) )
		return false;

	// Recursive if we got a ,
	if ( !parserCheckChar(p, ',') )
		return parseGroupContent(p, end, group);

	return true;
}

// Parse a group { ... }
static otGroup *parseGroup(char **p, char *end) {
	otGroup *group = NULL;

	if ( !parserSkipSpaces(p, end) )
		return NULL;

	if ( !parserCheckChar(p, '{') )
		INVALID_SYNTAX("'{' expected");

	group = new otGroup();

	if ( !parserSkipSpaces(p, end) )
		goto parse_error;

	if ( !parseGroupContent(p, end, group) )
		goto parse_error;

	if ( !parserSkipSpaces(p, end) )
		goto parse_error;

	if ( !parserCheckChar(p, '}') )
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

otModule* otParser::parseString(const char* str) {
	char *p = (char *)str,
		 *end = (char *)(str + strlen(str));
	return parseGroup(&p, end);
}
