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

/** WOscTimeTag source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-13 16:38:01 $
 * $Revision: 1.4 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscTimeTag.h"

// define largest possible time tag (on compiler basis)
#if defined( _MSC_VER )
#	define WOSCLIB_MAXTT_64		0xFFFFFFFFFFFFFFFF
#elif defined(__GNUC__)
#	define WOSCLIB_MAXTT_64		0xFFFFFFFFFFFFFFFFULL
#endif

/** Maximal value of an OSC time tag when implemented as two 32bit integers,
 * higer byte.                                                               */
#define WOSCLIB_MAXTT_32_H		0xFFFFFFFF;
/** Maximal value of an OSC time tag when implemented as two 32bit integers,
 * lower byte.                                                               */
#define WOSCLIB_MAXTT_32_L		0xFFFFFFFF;

/*  -----------------------------------------------------------------------  */
/* 	WOscSystemTime                                                           */
/*  -----------------------------------------------------------------------  */

#if WOSC_HAS_STD_TIME_LIB
#	include <time.h>
	/* 17 leap years, stolen from OSC-kit, no guarantee for correctness...	 */
#	if defined( _MSC_VER )
#		define SECONDS_FROM_1900_TO_1970 2208988800
#	elif defined(__GNUC__)
#		define SECONDS_FROM_1900_TO_1970 2208988800ULL
#	endif
#endif

/** Returns the system time.
 * When not overridden in a derived class, it returns largest time tag possible.
 * 
 * \returns
 * Time representing the current system time.
 *
 * \remarks
 * Time tags are represented by a 64 bit fixed point number. The first 32 bits specify
 * the number of seconds since midnight on January 1, 1900, and the last 32 bits specify
 * fractional parts of a second to a precision of about 200 picoseconds. This is the rep-
 * resentation used by Internet NTP timestamps. The time tag value consisting of 63 zero
 * bits followed by a one in the least signifigant bit is a special case meaning "immediately."
 */
WOscTimeTag WOscSystemTime::GetSystemTime() const {

#ifdef WOSCLIB_UINT64

#	if WOSC_HAS_STD_TIME_LIB
	/* The time.h lib states:
	 * "" time_t  time ( time_t * timer ); ""
	 * Get the number of seconds elapsed since 00:00 hours,
	 * Jan 1, 1970 UTC from the system clock.
	 */
	WOscTimeTag systemTime;
	systemTime.m_timeTag = (WOSCLIB_UINT64)time(NULL) + SECONDS_FROM_1900_TO_1970;
	return systemTime;

#	else

	return WOscTimeTag::GetLargestTimeTag();

#	endif
#else

	/** \todo 32 bit implementation. */

#endif
}


/*  -----------------------------------------------------------------------  */
/* 	WOscTimeTag                                                              */
/*  -----------------------------------------------------------------------  */

/** Constructor
 * Initializes the time-tag to immediate time.
 * 
 */
WOscTimeTag::WOscTimeTag(){

#ifdef WOSCLIB_UINT64
	m_timeTag = 1;
#else
	m_timeTagH = 0;
	m_timeTagL = 1;
#endif
}

/** Construct an OSC-time tag from a raw OSC (network) byte-
 * stream (buffer).
 * The OSC (network) buffer has to be big-endian
 * 
 * \param rawTimeTag
 * Buffer containing the raw time-tag
 * 
 * \remarks
 * The argument buffer has to have a minimum length of 
 * WOscTimeTag::Constants::TIME_TAG_SIZE. Smaller lengths
 * may produce runtime errors.
 */
WOscTimeTag::WOscTimeTag(const char* rawTimeTag){
	InitFromCharArray(rawTimeTag);
}

/** Copy constructor.
 * Initializes the current time-tag with the data from the referenced.
 * 
 * \param rhs
 * Reference of time-tag to be copied.
 */
WOscTimeTag::WOscTimeTag(const WOscTimeTag& rhs){
#ifdef WOSCLIB_UINT64
	m_timeTag = rhs.m_timeTag;
#else
	m_timeTagH = rhs.m_timeTagH;
	m_timeTagL = rhs.m_timeTagL;
#endif
}

/** Returns the current system time.
 * A optional system-time object can be passed, which implements
 * the system specific system-time-query.
 * 
 * \param systemTime
 * Pointer to system time object (optional). If not specified,
 * The system time gets initialized with the library default 
 * system time.
 *
 * \returns
 * Time-tag representing the current system time.
 */
WOscTimeTag WOscTimeTag::GetCurrentTime(const WOscSystemTime* systemTime /* = NULL*/){
	if ( systemTime == NULL ){
		/* get library default */
		WOscSystemTime time;
		return time.GetSystemTime();
	}else
		return systemTime->GetSystemTime();
}

/** Returns the "immediate" time-tag.
 * Immediate implies the LSB set to one. 
 * 
 * \returns
 * Immediate time-tag.
 */
WOscTimeTag WOscTimeTag::GetImmediateTime(){
	WOscTimeTag retTag;
#ifdef WOSCLIB_UINT64
	retTag.m_timeTag = 1;
#else
	retTag.m_timeTagH = 0;
	retTag.m_timeTagL = 1;
#endif
	return retTag;
}

/** Returns the largest possible time-tag.
 * I.e. all bits set to one.
 * 
 * \returns
 * Largest possible time-tag.
 */
WOscTimeTag WOscTimeTag::GetLargestTimeTag(){
	WOscTimeTag retTag;
#ifdef WOSCLIB_UINT64
	retTag.m_timeTag = WOSCLIB_MAXTT_64;
#else
	retTag.m_timeTagH = WOSCLIB_MAXTT_32_H;
	retTag.m_timeTagL = WOSCLIB_MAXTT_32_L;
#endif
	return retTag;
}

/** Returns the "immediate" time-tag.
 * Immediate implies the LSB set to one. 
 * 
 * \returns
 * Immediate time-tag.
 */
WOscTimeTag WOscTimeTag::GetSmallestTimeTag(){
	return GetImmediateTime();
}

/** Sets this time-tag to the current system time.
 * A optional system-time object can be passed, which implements
 * the system specific system-time-query.
 * 
 * \param systemTime
 * Pointer to system time object (optional). If not specified,
 * The system time gets initialized with the library default 
 * system time.
 *
 * \see
 * WOscTimeTag::getCurrentTime(const WOscSystemTime* systemTime)
 */
void WOscTimeTag::SetToCurrentTime(const WOscSystemTime* systemTime /*= NULL*/){
	*this = systemTime->GetSystemTime();
}

/** Sets this time tag to immediate time.
 * 
 * \see
 * WOscTimeTag::getImmediateTime()
 */
void WOscTimeTag::SetToImmediateTime(){
	*this = GetImmediateTime();
}

/** Sets this time tag to the biggest time-tag possible.
 * 
 * \see
 * WOscTimeTag::getLargestTimeTag()
 */
void WOscTimeTag::SetToLargestTimeTag(){
	*this = GetLargestTimeTag();
}

/** Sets this time tag to the smallest time-tag possible.
 * 
 * \see
 * WOscTimeTag::getSmallestTimeTag()
 */
void WOscTimeTag::SetToSmallestTimeTag(){
	*this = GetSmallestTimeTag();
}

/** Add operator.
 * Adds the right-hand time-tag to the left-hand time-tag
 * and returns the result without modifying the left-hand time-tag.
 *
 * \param rhs
 * Right-hand time-tag reference to be added to the left one.
 * 
 * \returns
 * Left-hand time-tag plus right-hand time-tag. 
 */
WOscTimeTag WOscTimeTag::operator+ (const WOscTimeTag& rhs) const{
	
	/** \todo 32 bit implementation. */
	
	WOscTimeTag retTag;
#ifdef WOSCLIB_UINT64
	retTag.m_timeTag = m_timeTag + rhs.m_timeTag;
#else

#endif
	return retTag;
}

/** Assign operator.
 * Assigns the right-hand time-tag to the left-hand time-tag
 * and returns it.
 *
 * \param rhs
 * Right-hand time-tag reference to be assigned to the left one.
 * 
 * \returns
 * Left-hand time-tag value which equals now the right-hand time-tag. 
 */
WOscTimeTag WOscTimeTag::operator= (const WOscTimeTag& rhs){
#ifdef WOSCLIB_UINT64
	m_timeTag = rhs.m_timeTag;
#else
	m_timeTagH = rhs.m_timeTagH;
	m_timeTagL = rhs.m_timeTagL;
#endif
	return *this;
}

/** Add and assign operator.
 * Adds the right-hand time-tag to the left-hand time-tag,
 * stores the result in the left-hand time-tag and 
 * returns the result.
 *
 * \param rhs
 * Right-hand time-tag reference to be added to the left one.
 * 
 * \returns
 * Left-hand time-tag plus right-hand time-tag. 
 */
WOscTimeTag WOscTimeTag::operator+= (const WOscTimeTag& rhs){

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	m_timeTag =+ rhs.m_timeTag;
#else

#endif
	return *this;
}

/** Subtract operator.
 * Subtracts the right-hand time-tag from the left-hand time-tag
 * and returns the result without modifying the left-hand time-tag.
 *
 * \param rhs
 * Right-hand time-tag reference to be subtracted from the left one.
 * 
 * \returns
 * Left-hand time-tag minus right-hand time-tag. 
 */
WOscTimeTag WOscTimeTag::operator- (const WOscTimeTag& rhs) const{

	/** \todo 32 bit implementation. */

	WOscTimeTag retTag;
#ifdef WOSCLIB_UINT64
	retTag.m_timeTag = m_timeTag - rhs.m_timeTag;
#else
	
#endif
	return retTag;
}

/** Subtract and assign operator.
 * Subtracts the right-hand time-tag from the left-hand time-tag,
 * stores the result in the left-hand time-tag and 
 * returns the result.
 *
 * \param rhs
 * Right-hand time-tag reference to be subtracted from the left one.
 * 
 * \returns
 * Left-hand time-tag minus right-hand time-tag. 
 */
WOscTimeTag WOscTimeTag::operator-= (const WOscTimeTag& rhs){

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	m_timeTag -= rhs.m_timeTag;
#else

#endif
	return *this;
}

/** Smaller or equal comparison operator.
 * Returns true if the left time-tag is smaller (older)
 * than the right.
 * 
 * \param rhs
 * Right-hand side.
 * 
 * \returns
 * True if the left time-tag is smaller (older)
 * than the right. False else.
 */
bool WOscTimeTag::operator< (const WOscTimeTag& rhs) const{

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	return (m_timeTag < rhs.m_timeTag);
#else

#endif
}

/** Smaller or equal comparison operator.
 * Returns true if the left time-tag is smaller (older)
 * or equal (same time) than the right.
 * 
 * \param rhs
 * Right-hand side.
 * 
 * \returns
 * True if the left time-tag is smaller (older)
 * or equal (same time) than the right. False else.
 */
bool WOscTimeTag::operator<= (const WOscTimeTag& rhs) const{

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	return (m_timeTag <= rhs.m_timeTag);
#else

#endif
}

/** Is equal operator.
 * Returns true if the right time-tag is equal to the left.
 * 
 * \param rhs
 * Right-hand side.
 * 
 * \returns
 * True if the right time-tag is equal to the left.
 * False else.
 */
bool WOscTimeTag::operator== (const WOscTimeTag& rhs) const{

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	return (m_timeTag == rhs.m_timeTag);
#else

#endif
}

/** Is not equal operator.
 * Returns true if the right time-tag is different from the left.
 * 
 * \param rhs
 * Right-hand side.
 * 
 * \returns
 * True if the right time-tag is different from the left.
 * False else.
 */
bool WOscTimeTag::operator!= (const WOscTimeTag& rhs) const{

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	return (m_timeTag != rhs.m_timeTag);
#else

#endif
}

/** Bigger comparison operator.
 * Returns true if the right time-tag is smaller (older)
 * than the left.
 * 
 * \param rhs
 * Right-hand side.
 * 
 * \returns
 * True if the right time-tag is smaller (older)
 * than the left. False else.
 */
bool WOscTimeTag::operator> (const WOscTimeTag& rhs) const{

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	return (m_timeTag > rhs.m_timeTag);
#else

#endif
}

/** Bigger or equal comparison operator.
 * Returns true if the right time-tag is smaller (older)
 * or equal (same time) than the left.
 * 
 * \param rhs
 * Right-hand side.
 * 
 * \returns
 * True if the right time-tag is smaller (older)
 * or equal (same time) than the left. False else.
 */
bool WOscTimeTag::operator>= (const WOscTimeTag& rhs) const{

	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
	return (m_timeTag >= rhs.m_timeTag);
#else

#endif
}

/** Returns a new char array of size TIME_TAG_SIZE filled with the
 * network aligned (big endian) timetag.
 * 
 * The caller has to free the allocated memory.
 *
 * \returns
 * The allocated char array filled with the
 * network aligned (big endian) timetag.
 * 
 * \remarks
 * The caller has to free the allocated memory.
 * 
 */
char* WOscTimeTag::ToCharArray(){

	/** \todo 32 bit and big-endian implementation. */

#ifdef WOSCLIB_UINT64
#	if IS_LITTLE_ENDIAN == 1
	char* rawTimeTag = new char[TIME_TAG_SIZE];
	//char* tagLittleEndian = (char*)(&m_timeTag);
	char* tagLittleEndian = reinterpret_cast<char*>(&m_timeTag);
	rawTimeTag[7] = tagLittleEndian[0]; // lowest byte sits at highest address in OSC protocol...
	rawTimeTag[6] = tagLittleEndian[1];
	rawTimeTag[5] = tagLittleEndian[2];
	rawTimeTag[4] = tagLittleEndian[3];
	rawTimeTag[3] = tagLittleEndian[4];
	rawTimeTag[2] = tagLittleEndian[5];
	rawTimeTag[1] = tagLittleEndian[6];
	rawTimeTag[0] = tagLittleEndian[7];
	return rawTimeTag;
#	else
	char* rawTimeTag = new char[TIME_TAG_SIZE];
	//char* tagLittleEndian = (char*)(&m_timeTag);
	char* tagLittleEndian = reinterpret_cast<char*>(&m_timeTag);
	rawTimeTag[7] = tagLittleEndian[7]; // lowest byte sits at highest address in OSC protocol...
	rawTimeTag[6] = tagLittleEndian[6];
	rawTimeTag[5] = tagLittleEndian[5];
	rawTimeTag[4] = tagLittleEndian[4];
	rawTimeTag[3] = tagLittleEndian[3];
	rawTimeTag[2] = tagLittleEndian[2];
	rawTimeTag[1] = tagLittleEndian[1];
	rawTimeTag[0] = tagLittleEndian[0];
	return rawTimeTag;
#	endif
#elif 0

#endif
}

/** Fill the given buffer with the network char array
 * representation of an OSC time tag.
 *
 * \param buffer
 * Caller supplied buffer with a minimum size of TIME_TAG_SIZE.
 * 
 * \remarks
 * The byte stream buffer has to have a minimum length of
 * TIME_TAG_SIZE.
 */
void WOscTimeTag::WriteToCharArray(char* buffer){

	/** \todo 32 bit and big-endian implementation. */

#ifdef WOSCLIB_UINT64
#	if IS_LITTLE_ENDIAN == 1
	char* tagLittleEndian = reinterpret_cast<char*>(&m_timeTag);
	buffer[7] = tagLittleEndian[0]; // lowest byte sits at highest address in OSC protocol...
	buffer[6] = tagLittleEndian[1];
	buffer[5] = tagLittleEndian[2];
	buffer[4] = tagLittleEndian[3];
	buffer[3] = tagLittleEndian[4];
	buffer[2] = tagLittleEndian[5];
	buffer[1] = tagLittleEndian[6];
	buffer[0] = tagLittleEndian[7];
#	else
	char* tagLittleEndian = reinterpret_cast<char*>(&m_timeTag);
	buffer[7] = tagLittleEndian[7]; // lowest byte sits at highest address in OSC protocol...
	buffer[6] = tagLittleEndian[6];
	buffer[5] = tagLittleEndian[5];
	buffer[4] = tagLittleEndian[4];
	buffer[3] = tagLittleEndian[3];
	buffer[2] = tagLittleEndian[2];
	buffer[1] = tagLittleEndian[1];
	buffer[0] = tagLittleEndian[0];
#	endif
#elif 0

#endif
}

#include <string.h>

/** Initializes an OSC time tag from a byte stream (big endian
 * ordered).
 * The byte stream buffer has to have a minimum length of
 * TIME_TAG_SIZE.
 *
 * \param buffer
 * Pointer to buffer with initialization data.
 * 
 * \remarks
 * The byte stream buffer has to have a minimum length of
 * TIME_TAG_SIZE.
 */
void
WOscTimeTag::InitFromCharArray(const char* buffer)
{
	/** \todo 32 bit implementation. */

#ifdef WOSCLIB_UINT64
#	if IS_LITTLE_ENDIAN == 1
	/*
	char tmp[8];
	tmp[0] = buffer[7]; // lowest byte sits at highest address in OSC protocol...
	tmp[1] = buffer[6];
	tmp[2] = buffer[5];
	tmp[3] = buffer[4];
	tmp[4] = buffer[3];
	tmp[5] = buffer[2];
	tmp[6] = buffer[1];
	tmp[7] = buffer[0];
	m_timeTag = *((WOSCLIB_UINT64*)tmp);
	*/
	for ( int i = sizeof(WOSCLIB_UINT64)-1; i >= 0; i-- )
		((char*)&m_timeTag)[i] = buffer[sizeof(WOSCLIB_UINT64)-1-i];

#	else
/*
	char tmp[8];
	tmp[0] = buffer[0]; // lowest byte sits at highest address in OSC protocol...
	tmp[1] = buffer[1];
	tmp[2] = buffer[2];
	tmp[3] = buffer[3];
	tmp[4] = buffer[4];
	tmp[5] = buffer[5];
	tmp[6] = buffer[6];
	tmp[7] = buffer[7];
	m_timeTag = *((WOSCLIB_UINT64*)tmp);
*/
	m_timeTag = *reinterpret_cast<const WOSCLIB_UINT64*>(buffer);
#	endif
#elif 0

#endif
}
