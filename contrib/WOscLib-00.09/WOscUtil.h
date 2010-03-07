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

/** WOscUtil header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-05-16 21:21:41 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

#ifndef __WOSCUTIL_H__
#define __WOSCUTIL_H__

#include "WOscLib.h"

/** Weiss OSC library utilities.
 * Mostly buffer-to-type and type-to-buffer conversions,
 * big/little-endianess issues and 4-byte-boundary
 * utilities.
 * 
 */
class WOSC_EXPORT WOscUtil{
public:
	static int GetSizeFourByteAligned(const char* string);
	static int GetSizeFourByteAligned(const int length);

	static void PadStringWithZeros(char* destString, const char* sourceString);
	static void PadBufferWithZeros(char* destBuffer, const char* sourceBuffer, int destLen, int srcLen);

	static void WriteBuffer(char* buffer, int arg);
	static void WriteBuffer(char* buffer, float arg);
	static int CharBufToInt(const char* buf);
	static float CharBufToFloat(const char* buf);

	/** Constants used in WOscUtil
	 * OSC-argument sizes.
	 * 
	 * \remarks
	 * Osc arguments are platform independent.
	 */
	enum Constants{
		OSC_INT_SIZE	= 4,	/**< OSC-argument integer size.	*/
		OSC_FLOAT_SIZE	= 4,	/**< OSC-argument float size (Changed from 8 to 4,
								 * pointed out by Damian Stewart) .	*/
	};
protected:

private:

};

#endif	// #ifndef __WOSCUTIL_H__
