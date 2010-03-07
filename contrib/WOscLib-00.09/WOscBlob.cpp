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

/** WOscBlob source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-10 15:12:12 $
 * $Revision: 1.4 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscBlob.h"
#include "WOscString.h"
#include "WOscException.h"
#include "WOscUtil.h"
#include <string.h>


/** Constructor for WOscBlobs from a binary buffer containing an WOscBlob
 * in its binary representation ( [blobsize][blobdata] ).
 * 
 * \param binaryData
 * Pointer to the buffer containing the raw data.
 * 
 * \remarks
 * Does not handle illegal blob sizes when passing wrong buffers.
 * 
 */
WOscBlob::WOscBlob(const char* binaryData)
{
	// get blob size
	m_dataLen = WOscUtil::CharBufToInt( binaryData );
	m_bufferLen = WOscUtil::GetSizeFourByteAligned( m_dataLen ) + BLOB_SIZE_SIZE;
	m_buffer = new char[m_bufferLen] ;

	// write data to buffer 
	memcpy(m_buffer, binaryData, m_bufferLen);
}

/** Returns a pointer to the blob data. Get the length of the buffer with
 * WOscBlob::GetDataLength() .
 *
 * \returns
 * Pointer to the buffer containing the data.
 */
const char* WOscBlob::GetData() const
{
	return m_buffer + BLOB_SIZE_SIZE;
}


/** Constructs a WOscBlob-object from a binary buffer.
 * The buffer is internally rounded up to a multiple of 4.
 * If querying fo buffer size, the padded size is returned.
 * To get the actual data length, use the GetDataLen() function.
 *
 * \param binaryData
 * Buffer with data of length "dataLen".
 * 
 * \param dataLen
 * Length of data supplied with "binaryData".
 * 
 * \throws None.
 * 
 * \remarks
 * Check general information at WOscBlob .
 *
 *
 * \verbatim
 
 +-------------------+----//---------------------+
 | size of blob data | blob data + padding zeros |
 +-------------------+---------------------------+

 |----- 4 bytes -----|-- size rounded up bytes --|

   \endverbatim
 * 
 */
WOscBlob::WOscBlob(const char* binaryData, int dataLen){
	m_dataLen = dataLen;

	/* round up and allocate */
	m_bufferLen = WOscUtil::GetSizeFourByteAligned(dataLen) + BLOB_SIZE_SIZE;
	m_buffer = new char[m_bufferLen];
	
	/* write size to buffer */
	WOscUtil::WriteBuffer(m_buffer, dataLen);
	
	/* write data to buffer */
	memcpy(m_buffer + BLOB_SIZE_SIZE, binaryData, m_dataLen);

	/* zeropad buffer */
	for ( int i = m_dataLen + BLOB_SIZE_SIZE; i < m_bufferLen; i++ )
		m_buffer[i] = 0;
}

/** Assignment operator.
 * Copies the content of the referenced blob into this one.
 * 
 * \param rhs
 * Reference to another blob (right hand side of operator).
 * 
 * \throws None.
 * 
 * \remarks
 * None.
 */
WOscBlob& WOscBlob::operator= (WOscBlob &rhs){
	//copy(&rhs);
	if ( m_buffer )	delete [] m_buffer;
	m_dataLen = rhs.m_dataLen;
	m_bufferLen = rhs.m_bufferLen;
	m_buffer = new char[m_bufferLen];
	memcpy(m_buffer, rhs.m_buffer, m_bufferLen);
	
	return *this;
}

/** Pointer-copy-constructor.
 * Copies the content of the referenced blob into this one.
 * 
 * \param b
 * Reference to another blob.
 * 
 * \throws None.
 * 
 * \remarks
 * None.
 */
WOscBlob::WOscBlob(WOscBlob* b){
	m_buffer = NULL;
	*this = *b;
}

/** Copy-constructor.
 * Copies the content of the referenced blob into this one.
 * 
 * \param b
 * Reference to another blob.
 * 
 * \throws None.
 * 
 * \remarks
 * None.
 */
WOscBlob::WOscBlob(WOscBlob& b){
	m_buffer = NULL;
	*this = b;
}

/** Deletes the WOscBlob-object.
 * The internal buffer gets deleted.
 * 
 * \throws None.
 * 
 * \remarks
 * None.
 */
WOscBlob::~WOscBlob(){
	if ( m_buffer )
		delete [] m_buffer;
}

/** Returns the length of the data without zero-padding.
 * 
 * \returns
 * Size without zeros.
 * 
 * \see
 * GetBufferLen()
 */
int WOscBlob::GetDataLen() const{
	return m_dataLen;
}

/** Returns the buffer-length of the formatted buffer. 
 * This length includes zero-padding and size of the blob size.
 * 
 * \returns
 * Size of the formatted buffer.
 * 
 * \see
 * GetDataLen()
 */
int WOscBlob::GetBufferLen() const{
	return m_bufferLen;
}

/** Fills the buffer with the raw bytestream of this blob.
 * The user must supply the buffer of apropriate size.
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
 * when modifying the blob-object afterwards. (contrary to
 * WOscBlob::getBuffer() )
 * 
 * \see
 * WOscBlob::getBuffer()
 */
void WOscBlob::GetBuffer(char* buffer, int bufferLen) const{
	if ( bufferLen < m_bufferLen ){
		throw new WOscException(
			WOscException::ERR_BUFFER_TO_SMALL,
			"WOscBlob: Buffer too small when trying to copy data to buffer.");
	}else{
		memcpy(buffer, m_buffer, m_bufferLen);
	}
}

/** Returns a pointer to an internal buffer containing
 * the current binary representation of this blob.
 * Please see remarks for safety issues.
 * 
 * \returns
 * Pointer to internal array data.
 * 
 * \throws None.
 * 
 * \remarks
 * The pointer remains valid until the modification of this blob-object.
 * The next time the blob is destination of an operation (check operators) or
 * the blob will be deleted this pointer gets invalid. If you are
 * not sure about that issue, please use the other getBuffer()- function.
 * 
 * \see
 * WOscBlob::getBuffer(char* buffer, int bufferLen)
 */
const char* WOscBlob::GetBuffer() const{
	return m_buffer;
}


