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

/** WOscString source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-03-20 19:41:37 $
 * $Revision: 1.2 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscString.h"
#include <string.h>
#include "WOscUtil.h"
#include "WOscException.h"


/** Constructs an empty OSC-string-object.
 * The returned buffer will be empty.
 */
WOscString::WOscString(){
	m_buffer = NULL;
	m_bufferLen = 0;
}


/**	Constructs an OSC-String from a string.
 * Aligns it internally to a 4-byte boundary.
 * 
 * \param string
 * Initializing string.
 */
WOscString::WOscString(const char* string){
	/* get four byte aligned size */
	m_bufferLen = WOscUtil::GetSizeFourByteAligned(string);
	/* allocate mem */
	m_buffer = new char[m_bufferLen];
	/* copy and pad */
	WOscUtil::PadStringWithZeros(m_buffer, string);
}


/** Copy constructor.
 * Allocates new memory and copies the aligned string
 * from the referenced object.
 * 
 * \param rhs
 * Reference of an OSC-string object.
 */
WOscString::WOscString(const WOscString& rhs){
	/*  allocate */
	m_buffer = new char[m_bufferLen = rhs.m_bufferLen];
	/* copy */
	memcpy(m_buffer,rhs.m_buffer,m_bufferLen); 
}

/** Destructor.
 * Frees the memory required by the internal buffer (if not empty).
 */
WOscString::~WOscString(){
	if ( m_buffer )
		delete [] m_buffer;
}

/** OSC string char assignment operator.
 * Assigns the right-hand char to the OSC string object.
 * 
 * \param rhs
 * Char beeing assigned to the current OSC string.
 * 
 * \returns
 * Reference of the current OSC string object.
 * 
 */
WOscString& WOscString::operator= (const char rhs){
	if ( m_buffer ){
		delete [] m_buffer;
		m_buffer = NULL;
	}
	return *this += rhs;
}

/** String to OSC-String assignment operator.
 * Formats (padding) the right-hand string and copies
 * it into the left-hand object. Frees the old memory and
 * allocates new.
 *
 * \param rhs
 * Reference of the source string.
 * 
 * \returns
 * Reference of the destination object.
 */
WOscString& WOscString::operator= (const char* rhs){
	if ( m_buffer )
		delete [] m_buffer;

	/* get four byte aligned size */
	m_bufferLen = WOscUtil::GetSizeFourByteAligned(rhs);
	/* allocate mem */
	m_buffer = new char[m_bufferLen];
	/* copy and pad */
	WOscUtil::PadStringWithZeros(m_buffer, rhs);
	
	return *this;
}

/** Assignment operator.
 * Copies the right-hand object-reference into the
 * left-hand object. Frees the old memory and
 * allocates new.
 * 
 * \param rhs
 * Reference of the source object.
 * 
 * \returns
 * Reference of the destination object.
 */
WOscString& WOscString::operator= (const WOscString& rhs){
	if ( m_buffer )
		delete [] m_buffer;

	m_buffer = new char[m_bufferLen = rhs.m_bufferLen];
	memcpy(m_buffer,rhs.m_buffer,m_bufferLen);

	return *this;
}

/** OSC-String concatenation without assignment.
 * Appends the right hand object to the left-hand object but 
 * does not save it in the left-hand object.
 * The zeros from the left hand object are removed and the
 * resulting OSC-string is padded to a multiple of 4.
 *
 * \param rhs
 * Reference to an object to append.
 * 
 * \returns
 * The concatenated object.
 *
 * \remarks
 * Since the operator does not assign, the
 * return value can not be a reference, because
 * its an local and automatic variable.
 * 
 */
WOscString WOscString::operator+ (const WOscString& rhs){
	WOscString retVal;
	if ( m_buffer ){
		retVal += *this;
		retVal += rhs;
	}else{
		retVal = rhs;
	}
	return retVal;
}

/** Character to OSC-String concatenation with assignment.
 * Appends the right hand char to the left-hand object and 
 * saves it in the left-hand object.
 * The zeros from the left hand object are removed and the
 * resulting OSC-string is padded to a multiple of 4.
 *
 * \param rhs
 * Character to be appended to the left hand object.
 * 
 * \returns
 * The reference to the concatenated object.
 */
WOscString& WOscString::operator+= (const char rhs){
	char str[] = {rhs,'\0'};
	return *this += str;
}

/** String to OSC-String concatenation with assignment.
 * Formats and appends the right hand string to the left-hand object and 
 * saves it in the left-hand object.
 * The zeros from the left hand object are removed and the
 * resulting OSC-string is padded to a multiple of 4.
 *
 * \param rhs
 * String to be appended to the left hand object.
 * 
 * \returns
 * The reference to the concatenated object.
 */
WOscString& WOscString::operator+= (const char* rhs){
	WOscString tmp(rhs);
	return *this += tmp;
}

/** OSC-String concatenation with assignment.
 * Appends the right hand object to the left-hand object and 
 * saves it in the left-hand object.
 * The zeros from the left hand object are removed and the
 * resulting OSC-string is padded to a multiple of 4.
 *
 * \param rhs
 * Reference to an object to append.
 * 
 * \returns
 * The a reference to the concatenated object.
 */
WOscString& WOscString::operator+= (const WOscString& rhs){

	if ( m_buffer ){

		/* get length of both strings */
		int lenLhs = (int)strlen(m_buffer);
		int lenRhs = (int)strlen(rhs.m_buffer);

		/* get the size of the combined string aligned to four. */
		int newLen = WOscUtil::GetSizeFourByteAligned(lenRhs + lenLhs + 1);

		/* allocate memory */
		char* newBuf = new char[newLen];

		/* copy left and right hand side */
		memcpy(newBuf, m_buffer, lenLhs); 
		memcpy(newBuf+lenLhs, rhs.m_buffer, lenRhs);

		/* pad rest with zeros */
		for ( int i = lenLhs + lenRhs; i < newLen; i++ )
			newBuf[i] = '\0';

		delete [] m_buffer;		// delete old buffer
		m_buffer = newBuf;		// set new string buffer as buffer
		m_bufferLen = newLen;	// set length

	}else{
		/* if empty, just assign */
		*this = rhs;
	}
	return *this;
}

/** Returns the size of the zero-padded OSC-string.
 * Can be used to allocate memory for instance.
 * 
 * \returns
 * Size of the internal buffer.
 * 
 * \remarks
 * Remains valid until the content of the WOscString-object is changed. See 
 */
int WOscString::GetSize(){
	return m_bufferLen;
}

/** Fills the buffer with the internal (padded) OSC-string representation.
 * The user must supply a buffer of apropriate size.
 *
 * \param buffer
 * Caller supplied destination buffer.
 * 
 * \param bufferLen
 * Length of the supplied buffer.
 * 
 * \throws WOscException
 * When caller supplied buffer too small.
 * 
 * \remarks
 * This interface makes sure that the buffer remains valid
 * when modifying the WOscString-object afterwards. (contrary to
 * WOscString::getBuffer() )
 * 
 * \see
 * WOscString::getBuffer()
 */
void WOscString::GetBuffer(char* buffer, int bufferLen){
	if ( bufferLen < m_bufferLen ){
		throw new WOscException(
			WOscException::ERR_OSC_STR_BUFF_TOO_SMALL,
			"WOscString: Buffer too small when trying to copy string to buffer.");
	}else{
		for ( int i = 0; i < bufferLen; i++ )
			buffer[i] = m_buffer[i];
	}
}

/** Returns a pointer to an internal OSC-string-buffer.
 * Please See remarks for safety issues.
 * 
 * \returns
 * Pointer to the internal OSC-string buffer.
 * 
 * \remarks
 * The pointer remains valid until the modification of this WOscString-object.
 * The next time the object is destination of an operation (check operators)
 * or the object will be deleted, this pointer possibly gets invalid. If you are
 * not sure about that issue, please use the other getBuffer()- function, which 
 * copies the string.
 * 
 * \see
 * WOscString::getBuffer(char* buffer, int bufferLen)
 */
const char* const WOscString::GetBuffer(){
	return m_buffer;
}


/** Comparison operator.
 * Compares the right-hand OSC string reference with the current OSC object.
 * 
 * \param rhs
 * Reference of an OSC string object.
 * 
 * \returns
 * True when equal, false when not equal.
 * 
 */
bool WOscString::operator == (const WOscString& rhs) const{
	return strcmp(m_buffer, rhs.m_buffer) == 0 ? true : false;
}

