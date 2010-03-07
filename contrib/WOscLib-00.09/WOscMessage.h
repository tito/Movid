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

/** WOscMessage header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-13 16:38:01 $
 * $Revision: 1.4 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

#ifndef __WOSCMESSAGE_H__
#define __WOSCMESSAGE_H__

#include "WOscLib.h"
#include "WOscBlob.h"
#include "WOscPacket.h"
#include "WOscString.h"

/** OSC-message containing type-tag and OSC-arguments.
 * 
 * Osc arguments can be added by using the add function. 
 * 
 * \remarks
 * The order of the added items can be reconstructed: The first
 * integer added will be accessable with the first index when
 * using getInt(int idx)
 * 
 * 
 */
class WOSC_EXPORT WOscMessage: public WOscPacket{
public:
	WOscMessage(const char* address);
	WOscMessage(const char* buffer, int bufferLen);

	WOscMessage(const WOscMessage* message);
	WOscMessage(const WOscMessage& rhs);
	WOscMessage& operator= (const WOscMessage& rhs);
	WOscMessage& operator+= (const WOscMessage& rhs);
	WOscMessage operator+ (const WOscMessage& rhs);

	virtual ~WOscMessage();

	void Add(float f);
	void Add(int intArg);
	void Add(const char* s);
	void Add(WOscString& s);
	void Add(WOscBlob* b);

	int GetNumFloats() const;
	int GetNumInts() const;
	int GetNumStrings() const;
	int GetNumBlobs() const;

	WOscString GetOscAddress() const;
	
	float GetFloat(int idx) const;
	int GetInt(int idx) const;
	WOscString& GetString(int idx) const;
	WOscBlob* GetBlob(int idx) const;

	void GetBuffer(char* buffer, int bufferLen);
	virtual const char* GetBuffer();
	int GetBufferLen();

private:
	void Copy(const WOscMessage &msg);
	WOscMessage(); /**< disabled default constructor*/

	void GenerateBufferFromMembers();

	WOscString	m_address;		/**< OSC-address of this message.*/
	WOscString	m_typeTag;		/**< Type-tag of this message.*/

	float*		m_floats;		/**< Array of the floating-point arguments*/
	int			m_numFloats;	/**< Number of floating-point arguments (size of the array)*/
	int*		m_ints;			/**< Array of the integer arguments*/
	int			m_numInts;		/**< Number of integer arguments (size of the array)*/
	WOscString*	m_strings;		/**< Array of the OSC-string arguments*/
	int			m_numStrings;	/**< Number of OSC-string arguments (size of the array)*/
	WOscBlob**	m_blobs;		/**< Array of the OSC-blob arguments*/
	int			m_numBlobs;		/**< Number of OSC-blob arguments (size of the array)*/

	int			m_bufferLen;	/**< Length of the binary representation of this message.*/
	char*		m_buffer;		/**< Binary representation of this message.*/

};

#endif	// #ifndef __WOSCMESSAGE_H__
