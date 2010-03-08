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

/** WOscString header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-05-16 21:21:41 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCSTRING_H__
#define __WOSCSTRING_H__

#include "WOscLib.h"


/** Wrapping of OSC specific strings (4-byte aligned).
 * OSC-strings must be 4-byte aligned. The interface
 * of this class guarantees the 4-byte-alignment.
 *
 * \remarks
 * None.
 */
class WOSC_EXPORT WOscString{
public:
	WOscString();
	WOscString(const char* string);
	WOscString(const WOscString& rhs);
	~WOscString();

	/* assignment operators */
	WOscString& operator= (const char rhs);
	WOscString& operator= (const char* rhs);
	WOscString& operator= (const WOscString& rhs);
	WOscString operator+ (const WOscString& rhs);
	WOscString& operator+= (const char rhs);
	WOscString& operator+= (const char* rhs);
	WOscString& operator+= (const WOscString& rhs);

	/* logical operators */
	bool operator == (const WOscString& rhs) const ;


	int GetSize();
	void GetBuffer(char* buffer, int bufferLen);
	const char* const GetBuffer(); 

private:
	char*	m_buffer;		/**< Internal zero padded data buffer.*/
	int		m_bufferLen;	/**< Internal zero padded buffer length.*/
	
};

#endif	// #ifndef __WOSCSTRING_H__
