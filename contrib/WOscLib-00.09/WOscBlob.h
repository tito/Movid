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

/** WOscBlob header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-10 10:55:11 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCBLOB_H__
#define	__WOSCBLOB_H__

#include "WOscLib.h"

/** Encapsulates an OSC-blob for transmission of binary data.
 * Used to transport proprietary data over the OSC-protocol.
 * Inherit your own class from this object, for instance
 * a firmware-packet class, which could transport binary 
 * data fo firmware updates.
 * 
 * \remarks
 * When using datagram transmission, make sure that the
 * receive-buffer of the other peer can handle your
 * OSC-packet size (most systems can handle UDP datagram-packets
 * of sizes up to 512bytes)
 * 
 *
 * \todo
 * Change GetBufferLen to getDataLenZeroPadded
 *
 */
class WOSC_EXPORT WOscBlob{
public:
	WOscBlob(const char* binaryData);
	WOscBlob(const char* binaryData, int dataLen);
	WOscBlob(WOscBlob* b);
	WOscBlob(WOscBlob& b);
	virtual ~WOscBlob();
	
	WOscBlob& operator= (WOscBlob &rhs);

	int GetDataLen() const;
	const char* GetData() const;

	int GetBufferLen() const;
	void GetBuffer(char* buffer, int bufferLen) const;
	const char* GetBuffer() const;

	enum Constants{
		BLOB_SIZE_SIZE = 4,		/**< Size of blob's size. */
	};

private:
	WOscBlob(); // disable default constructor

	int		m_dataLen;		/**< Length of data without zero padding.*/
	char*	m_buffer;		/**< Data zero padded.*/
	int		m_bufferLen;	/**< m_dataLen Rounded up to next multiple of 4.*/
	
};

#endif	// #ifndef __WOSCBLOB_H__
