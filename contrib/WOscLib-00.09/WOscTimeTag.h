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

/** WOscTimeTag header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-14 17:16:49 $
 * $Revision: 1.6 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCTIMETAG_H__
#define __WOSCTIMETAG_H__

#include "WOscLib.h"
#include <stdlib.h>	// for NULL

class WOscTimeTag;

/*  -----------------------------------------------------------------------  */
/* 	WOscSystemTime                                                           */
/*  -----------------------------------------------------------------------  */

/** Implementation of the system time.
 * Every system using this library can inherit its own specific
 * class and override the getSystemTime() - member-function.
 * If not overridden (using the base class (this one :)), see 
 * WOscSystemTime::GetSystemTime() for details.
 *
 */
class WOSC_EXPORT WOscSystemTime{
public:
	virtual ~WOscSystemTime() {}
	virtual WOscTimeTag GetSystemTime() const;
};


/*  -----------------------------------------------------------------------  */
/* 	WOscTimeTag                                                              */
/*  -----------------------------------------------------------------------  */

/** Definition of time in an OSC system.
 * The OSC-specification ( \ref WOscLibOsc_spec_page "OSC specifications" ) states:
 * Time tags are represented by a 64 bit fixed point number. The first 32 bits specify
 * the number of seconds since midnight on January 1, 1900, and the last 32 bits specify
 * fractional parts of a second to a precision of about 200 picoseconds. This is the rep-
 * resentation used by Internet NTP timestamps. The time tag value consisting of 63 zero
 * bits followed by a one in the least signifigant bit is a special case meaning "immediately."
 * 
 * \remarks
 * This is one of the platform (compiler/processor/operating-system) dependent
 * parts of the library. WOscTimeTag is affected regarding the number-
 * representation (32/64 datatypes supported) and the endianness (big-
 * endian or little-endian). Both can be switched by preprocessor-defines.
 * 
 * \see
 * WOscSystemTime
 */
class WOSC_EXPORT WOscTimeTag{
	
public:
	WOscTimeTag();
	WOscTimeTag(const char* rawTimeTag);
	WOscTimeTag(const WOscTimeTag& rhs);

	static WOscTimeTag GetCurrentTime(const WOscSystemTime* systemTime = NULL);
	static WOscTimeTag GetImmediateTime();
	static WOscTimeTag GetLargestTimeTag();
	static WOscTimeTag GetSmallestTimeTag();

	void SetToCurrentTime(const WOscSystemTime* systemTime = NULL);
	void SetToImmediateTime();
	void SetToLargestTimeTag();
	void SetToSmallestTimeTag();

	WOscTimeTag operator+ (const WOscTimeTag& rhs) const;
	WOscTimeTag operator+= (const WOscTimeTag& rhs);
	WOscTimeTag operator= (const WOscTimeTag& rhs);
	WOscTimeTag operator- (const WOscTimeTag& rhs) const;
	WOscTimeTag operator-= (const WOscTimeTag& rhs);

	bool operator< (const WOscTimeTag& rhs) const;
	bool operator<= (const WOscTimeTag& rhs) const;
	bool operator== (const WOscTimeTag& rhs) const;
	bool operator!= (const WOscTimeTag& rhs) const;
	bool operator> (const WOscTimeTag& rhs) const;
	bool operator>= (const WOscTimeTag& rhs) const;

	char* ToCharArray();
	void WriteToCharArray(char* buffer);
	void InitFromCharArray(const char* buffer);

	/** Time-tag related constants.
	 * \remarks
	 * Use TIME_TAG_SIZE when referring to char-buffers
	 * related with the WOscTimeTag.
	 */
	enum Constants{
		TIME_TAG_SIZE = 8, /**< buffer size for char-arrayed timetags.*/
	};

private:
	friend class WOscSystemTime;

#ifdef WOSCLIB_UINT64
	WOSCLIB_UINT64 m_timeTag;	/**< Binary time-tag representation for platforms 
						 * with 8-byte integers.
						 */
#else
	WOSCLIB_UINT32 m_timeTagH;	/**< High-word of time-tag representation for platforms
						 * with 4-byte integers.
						 */
	WOSCLIB_UINT32 m_timeTagL;	/**< Low-word of time-tag representation for platforms
						 * with 4-byte integers.
						 */
#endif

};

#endif	// #ifndef __WOSCTIMETAG_H__
