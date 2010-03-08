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

/** WOscUtil source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-13 16:38:01 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscUtil.h"
#include "WOscLib.h"
#include <string.h>

/**  Get length of string and round up to the next multiple of 4.
 * Gets the size of the string passed as parameter and rounds it
 * up to the next multiple of four.
 * 
 * \param string
 * String which size should be rounded.
 * 
 * \returns
 * Rounded size of the string.
 * 
 * \see
 * WOscUtil::getSizeFourByteAligned(const int length)
 */
int WOscUtil::GetSizeFourByteAligned(const char* string){
	int length = (int)strlen(string) + 1;

	return length + ( 4 - length % 4 ) % 4;
}

// 
/** Rounds up the given integer to the next multiple of 4.
 * Similar to getSizeFourByteAligned(const char* string).
 * 
 * \param length
 * Integer which sould be rounded up to the next multiple of 4.
 * 
 * \returns
 * The rounded integer.
 * 
 * \see
 * getSizeFourByteAligned(const char* string)
 */
int WOscUtil::GetSizeFourByteAligned(const int length){
	return length + ( 4 - length % 4 ) % 4;
}


/**	Copies the source string to the destination string
 * and pads it with zeros to the next multiple of four, 
 * the destination buffer must be large enough.
 *
 * \param destString
 * Destination string buffer (length must be the rounded-up
 * length of the source string), which receives the padded
 * string.
 * 
 * \param sourceString
 * The string which should be padded.
 * 
 * \remarks
 * Works only with strings!!!
 * 
 * \see
 * For determination of the destination string-buffer-length:
 * getSizeFourByteAligned(const char* string)
 */
void WOscUtil::PadStringWithZeros(char* destString, const char* sourceString){
	int length = (int)strlen(sourceString) + 1;
	int osclength = GetSizeFourByteAligned(sourceString);

	strcpy(destString, sourceString);

	for ( int i = length; i < osclength; i++ )
		destString[i] = '\0';
}



/**	Copies the source buffer to the destination buffer
 * and pads the difference with zeros. 
 * The destination buffer must be large enough, i.e. destLen >= srcLen.
 *
 * Works for every buffer, assumed destLen >= srcLen. If destLen is smaller
 * than srcLen only srcLen data is copied and nothing padded.
 * 
 * \param destBuffer
 * Destination string buffer (length must be the rounded-up
 * length of the source string), which receives the padded
 * string.
 * 
 * \param sourceBuffer
 * Source buffer.
 * 
 * \param destLen
 * Size of the destination-buffer.
 * 
 * \param srcLen
 * Size of the source-buffer.
 * 
 * \see
 * padStringWithZeros(char* destString, const char* sourceString)
 */
void
WOscUtil::PadBufferWithZeros(char* destBuffer, const char* sourceBuffer,
		int destLen, int srcLen)
{
	if ( destLen <= srcLen )
		memcpy(destBuffer, sourceBuffer, destLen);
	else {
		memcpy(destBuffer, sourceBuffer, srcLen);
		memset(destBuffer+srcLen, 0, destLen-srcLen);
	}
}

/**	Writes an integer to a char buffer.
 * Takes care of endianess, when defined correctly in
 * WOscGlobal.h.
 *
 * \param buffer
 * Destination of the extracted integer.
 * 
 * \param arg
 * Integer to be written in buffer. The size of 
 * the destination buffer must be sufficient
 * ( WOscUtil::Constants see OSC_INT_SIZE).
 * 
 * \remarks
 * Every platform has to define its own version of
 * this routine.
 *
 * \see
 * charBufToInt(const char* buf) and
 * writeBuffer(char* buffer, float arg) or charBufToFloat(const char* buf) .
 * WOscUtil::Constants
 */
void WOscUtil::WriteBuffer(char* buffer, int arg){
#if IS_LITTLE_ENDIAN == 1
	/*
	char* tmp = (char*)(&arg);
	buffer[0] = tmp[3];
	buffer[1] = tmp[2];
	buffer[2] = tmp[1];
	buffer[3] = tmp[0];
	*/
	for (int i = sizeof(int)-1; i >= 0; i--)
		buffer[i] = ((char*)&arg)[sizeof(int)-1-i];

#else
	/*
	char* tmp = (char*)(&arg);
	buffer[0] = tmp[0];
	buffer[1] = tmp[1];
	buffer[2] = tmp[2];
	buffer[3] = tmp[3];
	*/
	memcpy(buffer, &arg, sizeof(int));
#endif
}

/**	Writes an float to a char buffer.
 * Takes care of endianess, when defined correctly in
 * WOscGlobal.h.
 *
 * \param buffer
 * Destination of the extracted float. The size of 
 * the destination buffer must be sufficient
 * ( WOscUtil::Constants see OSC_FLOAT_SIZE).
 * 
 * \param arg
 * Float to be written in buffer.
 * 
 * \remarks
 * Every platform has to define its own version of
 * this routine.
 *
 * \see
 * charBufToFloat(const char* buf) and
 * writeBuffer(char* buffer, int arg) or charBufToInt(const char* buf) ,
 * WOscUtil::Constants
 */
void WOscUtil::WriteBuffer(char* buffer, float arg){
#if IS_LITTLE_ENDIAN == 1
	/*
	char* tmp = (char*)(&arg);
	buffer[0] = tmp[3];
	buffer[1] = tmp[2];
	buffer[2] = tmp[1];
	buffer[3] = tmp[0];
	*/
	for (int i = sizeof(float)-1; i >= 0; i--)
		buffer[i] = ((char*)&arg)[sizeof(float)-1-i];
#else
	/*
	char* tmp = (char*)(&arg);
	buffer[0] = tmp[0];
	buffer[1] = tmp[1];
	buffer[2] = tmp[2];
	buffer[3] = tmp[3];
	*/
	memcpy(buffer, &arg, sizeof(float));
#endif
}

/**	Extracts an integer from a char buffer.
 * Takes care of endianess, when defined correctly in
 * WOscGlobal.h.
 *
 * \param buf
 * Extraction-source. The size of 
 * the source buffer must be sufficient
 * ( WOscUtil::Constants see OSC_INT_SIZE).
 * 
 * \returns
 * Integer extracted from buffer.
 * 
 * \remarks
 * Every platform has to define its own version of
 * this routine.
 *
 * \see
 * writeBuffer(char* buffer, int arg) and
 * writeBuffer(char* buffer, float arg) or charBufToFloat(const char* buf) ,
 * WOscUtil::Constants
 */
int
WOscUtil::CharBufToInt(const char* buf)
{
#if IS_LITTLE_ENDIAN == 1
	/*
	char tmp[4];
	tmp[0] = buf[3];
	tmp[1] = buf[2];
	tmp[2] = buf[1];
	tmp[3] = buf[0];
	return *((int*)tmp);
	*/
	int tmp;
	for ( int i = sizeof(int)-1; i >= 0; i-- )
		((char*)&tmp)[i] = buf[sizeof(int)-1-i];
	return tmp;
#else
	/*
	char tmp[4];
	tmp[0] = buf[0];
	tmp[1] = buf[1];
	tmp[2] = buf[2];
	tmp[3] = buf[3];
	return *((int*)tmp);
	*/
	return *reinterpret_cast<const int*>(buf);
#endif
}

/**	Extracts a float from a char buffer.
 * Takes care of endianess, when defined correctly in
 * WOscGlobal.h.
 *
 * \param buf
 * Extraction-source. The size of 
 * the source buffer must be sufficient
 * ( WOscUtil::Constants see OSC_FLOAT_SIZE).
 * 
 * \returns
 * Float extracted from buffer.
 * 
 * \remarks
 * Every platform has to define its own version of
 * this routine.
 *
 * \see
 * writeBuffer(char* buffer, float arg) and
 * writeBuffer(char* buffer, int arg) or charBufToInt(const char* buf) ,
 * WOscUtil::Constants
 */
float
WOscUtil::CharBufToFloat(const char* buf)
{
#if IS_LITTLE_ENDIAN == 1
	/*
	char tmp[4];
	tmp[0] = buf[3];
	tmp[1] = buf[2];
	tmp[2] = buf[1];
	tmp[3] = buf[0];
	return *((float*)tmp);
	*/
	float tmp;
	for ( int i = sizeof(float)-1; i >= 0; i-- )
		((char*)&tmp)[i] = buf[sizeof(float)-1-i];
	return tmp;
#else
	/*
	char tmp[4];
	tmp[0] = buf[0];
	tmp[1] = buf[1];
	tmp[2] = buf[2];
	tmp[3] = buf[3];
	return *((float*)tmp);
	*/
	return *reinterpret_cast<const float*>(buf);
#endif
}
