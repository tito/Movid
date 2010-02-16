#ifndef OT_PARSER_H
#define OT_PARSER_H

#include <string.h>

class otPipeline;

class otParser {
public:
	static otPipeline *parseString(std::string str);
};

#endif
