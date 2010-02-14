#ifndef OT_PARSER_H
#define OT_PARSER_H

class otModule;

class otParser {
public:
	static otModule *parseString(const char* str);
};

#endif
