/*
 * WOscLib, an object oriented OSC library.
 * Copyright (C) 2005 Uli Clemens Franke, Weiss Engineering LTD, Switzerland.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * For details see lgpl.txt
 *
 * Weiss Engineering LTD.
 * Florastrass 42
 * 8610 Uster
 * Switzerland
 *
 * uli.franke@weiss.ch
 */

/** WOscPatternMatch source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscPatternMatch.h"

// Matt's OSC-Kit:
//static const char *theWholePattern;	/* Just for warning messages */

/** Checks if the pattern matches agains the test-string.
 * Code from Matt Wright's OSC-Kit. Thanks...
 *
 * \param pattern
 * The pattern against test has to match.
 * 
 * \param test
 * The string to be tested.
 * 
 * \returns
 * True if both match regarding the definitions in the OSC-address-syntax
 * and OSC pattern matching. See the \ref WOscLibOsc_spec_page "OSC specifications"
 * for details.
 * 
 * \throws
 * Currently nothing. Could be changed, if desired.
 * 
 */
bool WOscPatternMatch::PatternMatch (const char *  pattern, const char * test) {
	// Matt's OSC-Kit:
	// theWholePattern = pattern;

	if (pattern == 0 || pattern[0] == 0) {
		return test[0] == 0;
	} 

	if (test[0] == 0) {
		if (pattern[0] == '*')
			return PatternMatch (pattern+1,test);
		else
			return false;
	}

	switch (pattern[0]) {
	case 0      : return test[0] == 0;
	case '?'    : return PatternMatch (pattern + 1, test + 1);
	case '*'    : 
		if (PatternMatch (pattern+1, test)) {
			return true;
		} else {
			return PatternMatch (pattern, test+1);
		}
	case ']'    :
	case '}'    :
		/* Can be changed to exception, if desired (does not make sense). In Matt's OSC-Kit it was:
		* OSCWarning("Spurious %c in pattern \".../%s/...\"",pattern[0], theWholePattern);
		*/
		return false;
	case '['    :
		return MatchBrackets (pattern,test);
	case '{'    :
		return MatchList (pattern,test);
	case '\\'   :  
		if (pattern[1] == 0) {
			return test[0] == 0;
		} else if (pattern[1] == test[0]) {
			return PatternMatch (pattern+2,test+1);
		} else {
			return false;
		}
	default     :
		if (pattern[0] == test[0]) {
			return PatternMatch (pattern+1,test+1);
		} else {
			return false;
		}
	}
}

/** Checks if the pattern matches agains the test-string.
 * Code from Matt Wright's OSC-Kit. Thanks...
 *
 * \param pattern
 * The pattern against test has to match.
 * 
 * \param test
 * The string to be tested.
 * 
 * \returns
 * True if both match regarding the definitions in the OSC-address-syntax
 * and OSC pattern matching. See the \ref WOscLibOsc_spec_page "OSC specifications"
 * for details.
 * 
 * \throws
 * Currently nothing. Could be changed, if desired.
 * 
 * \todo Write more precise documentation.
 */
/* we know that pattern[0] == '[' and test[0] != 0 */
bool WOscPatternMatch::MatchBrackets (const char *pattern, const char *test) {
	bool result;
	bool negated = false;
	const char *p = pattern;

	if (pattern[1] == 0) {
		/* Can be changed to exception, if desired (does not make sense). In Matt's OSC-Kit it was:
		 * OSCWarning("Unterminated [ in pattern \".../%s/...\"", theWholePattern);
		 */
		return false;
	}

	if (pattern[1] == '!') {
		negated = true;
		p++;
	}

	while (*p != ']') {
		if (*p == 0) {
			/* Can be changed to exception, if desired (does not make sense). In Matt's OSC-Kit it was:
			 * OSCWarning("Unterminated [ in pattern \".../%s/...\"", theWholePattern);
			 */
			return false;
		}
		if (p[1] == '-' && p[2] != 0) {
			if (test[0] >= p[0] && test[0] <= p[2]) {
				result = !negated;
				goto advance;
			}
		}
		if (p[0] == test[0]) {
			result = !negated;
			goto advance;
		}
		p++;
	}

	result = negated;

advance:

	if (!result)
		return false;

	while (*p != ']') {
		if (*p == 0) {
			/* Can be changed to exception, if desired (does not make sense). In Matt's OSC-Kit it was:
			 * OSCWarning("Unterminated [ in pattern \".../%s/...\"", theWholePattern);
			 */
			return false;
		}
		p++;
	}

	return PatternMatch (p+1,test+1);
}

/** Checks if the pattern matches against the test-string.
 * Code from Matt Wright's OSC-Kit. Thanks...
 *
 * \param pattern
 * The pattern against test has to match.
 * 
 * \param test
 * The string to be tested.
 * 
 * \returns
 * True if both match regarding the definitions in the OSC-address-syntax
 * and OSC pattern matching. See the \ref WOscLibOsc_spec_page "OSC specifications"
 * for details.
 * 
 * \throws
 * Currently nothing. Could be changed, if desired.
 * 
 * \todo Write more precise documentation.
 */
bool WOscPatternMatch::MatchList (const char *pattern, const char *test) {

	const char *restOfPattern, *tp = test;


	for(restOfPattern = pattern; *restOfPattern != '}'; restOfPattern++) {
		if (*restOfPattern == 0) {
			/* Can be changed to exception, if desired (does not make sense). In Matt's OSC-Kit it was:
			 * OSCWarning("Unterminated { in pattern \".../%s/...\"", theWholePattern);
			 */
			return false;
		}
	}

	restOfPattern++; /* skip close curly brace */

	pattern++; /* skip open curly brace */

	while (1) {

		if (*pattern == ',') {
			if (PatternMatch (restOfPattern, tp)) {
				return true;
			} else {
				tp = test;
				++pattern;
			}
		} else if (*pattern == '}') {
			return PatternMatch (restOfPattern, tp);
		} else if (*pattern == *tp) {
			++pattern;
			++tp;
		} else {
			tp = test;
			while (*pattern != ',' && *pattern != '}') {
				pattern++;
			}
			if (*pattern == ',') {
				pattern++;
			}
		}
	}
}

/** Advances in a string until a '\\0' or a '/' is found.
 * 
 * 
 * \param p
 * String to be scanned.
 * 
 * \returns
 * The next position of a '\\0' or a '/' in given string.
 * 
 */
const char* WOscPatternMatch::NextSlashOrNull(const char *p) {
	while (*p != '/' && *p != '\0') {
		p++;
	}
	return p;
}


