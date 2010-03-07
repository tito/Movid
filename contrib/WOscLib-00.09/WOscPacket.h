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

/** WOscPacket header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2007-11-30 17:00:12 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCPACKET_H__
#define __WOSCPACKET_H__


#include "WOscLib.h"

/** Interface definition of OSC-datatypes which are going to be
 * received and transferred through a network.
 *
 * WOscBundle and WOscMessage are such objects. Both can be elements of an OSC-
 * bundle.
 */
class WOSC_EXPORT WOscPacket{
public:
	virtual ~WOscPacket() {}
	/** Fills the buffer with the raw bytestream of this packet.
	 */
	virtual void GetBuffer(char* buffer, int bufferLen) = 0;
	/** Allocates memory and fills it with the content of the buffer,
	 * the caller has to delete it, the buffer-size can be queried
	 * through "getBufferLen()".
	 */
	virtual const char* GetBuffer() = 0;
	/** Returns the required buffer size when calling "getBuffer".
	 */
	virtual int GetBufferLen() = 0;
	
};

#endif	// #ifndef __WOSCPACKET_H__
