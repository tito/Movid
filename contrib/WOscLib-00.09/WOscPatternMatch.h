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

/** WOscPatternMatch header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.4 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCPATTERNMATCH_H__
#define __WOSCPATTERNMATCH_H__

#include "WOscLib.h"

/** Pattern matching algorithms.
 * All code stolen from Matt Wright's OSC-Kit.
 * Handles all tasks related to checking OSC-addresses against certain patterns.
 * 
 * \see
 * For information about the OSC address syntax see the \ref WOscLibOsc_spec_page "OSC specifications".
 */
class WOSC_EXPORT WOscPatternMatch{
	
public:
	static bool PatternMatch(const char *  pattern, const char * test);
	static const char* NextSlashOrNull(const char *p); 
protected:
	static bool MatchBrackets (const char *pattern, const char *test);
	static bool MatchList (const char *pattern, const char *test);
};

#endif	// #ifndef __WOSCPATTERNMATCH_H__
