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

/** WOscMessage source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-13 16:38:01 $
 * $Revision: 1.5 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscMessage.h"
#include <string.h>
#include "WOscString.h"
#include "WOscUtil.h"
#include "WOscException.h"

/** Type tag for empty OSC messages.                                         */
#define W_OSC_EMPTY_TYPE_TAG		",\0\0\0"
/** Length of the type tag for empty OSC messages.                           */
#define W_OSC_EMPTY_TYPE_TAG_LEN	4

/** Initialize an OSC-message with an OSC address.
 * Constructs an OSC message without arguments what produces an "empty"
 * type tag (",\0\0\0"). To add arguments, call add(int intArg) or similar.
 *
 * \param address
 * String with OSC address.
 * 
 * \throws WOscException
 * If address is NULL or if address does not begin with a slash.
 * 
 * \remarks
 * None.
 * 
 * \see
 * WOscMessage::add(float floatArg) etc.
 * 
 * \callgraph
 *
 * \todo
 * Check complete OSC-address-syntax
 */
WOscMessage::WOscMessage(const char* address){

	// check if valid address
	if ( address == NULL )
		throw new WOscException(
			WOscException::ERR_NULL_ADDRESS,
			"WOscMessage: No address (NULL) supplied during message construction.");

	if ( address[0] != '/' )
		throw new WOscException(
			WOscException::ERR_INVALID_ADDRESS_NO_SLASH,
			"WOscMessage: Initializing address does not begin with a slash.");

	// init address
	m_address = address;

	// init type tag
	m_typeTag = W_OSC_EMPTY_TYPE_TAG;

	m_floats		= NULL;
	m_numFloats		= 0;
	m_ints			= NULL;
	m_numInts		= 0;
	m_strings		= NULL;
	m_numStrings	= 0;
	m_blobs			= NULL;
	m_numBlobs		= 0;

	m_buffer = NULL;

	GenerateBufferFromMembers();
}

/** Initialize an OSC-message with a raw byte-stream
 * (for instance from a network receiver).
 * The message object is reassembled from binary data. The length
 * must be known. When successfully reassembled, the arguments
 * (when existing) are accessable via the getNumXXX() and getXXX(int idx)
 * interface functions.
 *
 * \param buffer
 * Packet data containing the whole message with arguments.
 * 
 * \param bufferLen
 * Packet data length.
 * 
 * \throws WOscException
 * When corrupt initialization data, inconsistent bufferlengths etc. found.
 * 
 * \todo Since a message is defined, when its beginning is known
 * creating it from a byte stream without knowing the length would be
 * possible. Write an initializer.
 * 
 * \remarks
 * If an exception occurs, the construction of the message will be 
 * aborted, including the arguments.
 *
 * \todo
 * Add intensive exception handling for corrupt init data
 *
 * \see
 * WOscException | WOscString
 */
WOscMessage::WOscMessage(const char* buffer, int bufferLen){

	m_floats		= NULL;
	m_numFloats		= 0;
	m_ints			= NULL;
	m_numInts		= 0;
	m_strings		= NULL;
	m_numStrings	= 0;
	m_blobs			= NULL;
	m_numBlobs		= 0;

	m_buffer = NULL;

	// check address
	if (bufferLen < 1)
		throw new WOscException(
			WOscException::ERR_NULL_BUFFER,
			"WOscMessage: Message initialization buffer is empty.");
	// check address
	if (buffer[0] != '/')
		throw new WOscException(
			WOscException::ERR_INVALID_ADDRESS_NO_SLASH,
			"WOscMessage: Message address corrupt, no slash.");

	// extract address
	m_address = buffer;

	// get type tag
	int pos = m_address.GetSize();

	m_typeTag = W_OSC_EMPTY_TYPE_TAG; // init type tag
	

	if (bufferLen <= pos){
		// no type tag in buffer -> leave empty type tag
		GenerateBufferFromMembers();
		return;
	}

	WOscString OscTypeTag = buffer + pos;

	// get args by scanning typetag string
	const char* typeTag = OscTypeTag.GetBuffer();
	int typeTagSize = OscTypeTag.GetSize();
	pos += typeTagSize; // offset in buffer (beginning of argument data)

	for ( int i = 0; i < typeTagSize; i++ ){
		switch (typeTag[i]){
			case 'i':
				Add(WOscUtil::CharBufToInt(buffer + pos));
				pos += WOscUtil::OSC_INT_SIZE;
				break;
			case 'f':
				Add(WOscUtil::CharBufToFloat(buffer + pos));
				pos += WOscUtil::OSC_FLOAT_SIZE;
				break;
			case 's':
				Add(buffer+pos);
				pos += m_strings[m_numStrings-1].GetSize();
				break;
			case 'b':{
				WOscBlob* blob = new WOscBlob(buffer + pos);
				Add( blob );
				pos += blob->GetBufferLen();
				delete blob;
				break;
					 }

			case ',':
				break;
			case '\0':
				i = typeTagSize; // jump to end
				break;
			default: // error
				i = typeTagSize; // jump to end
				break;
		}
	}

	GenerateBufferFromMembers(); // generate arrays of arguments for later use with getXXX(int idx)
}


/** Cleans up.
 * Since the OSC-address and the type-tag are automatic, only the
 * dynamic array members and buffers have to be cleaned up.
 */
WOscMessage::~WOscMessage(){

	if ( m_buffer )
		delete [] m_buffer;

	if (m_floats) delete [] m_floats;
	if (m_ints) delete [] m_ints;
	if (m_strings) delete [] m_strings;
	if (m_blobs){
		for(int i=0; i<m_numBlobs; i++)
			delete m_blobs[i];
		delete [] m_blobs;
	}

}

/** Copy constructor.
 * Initializes a new WOscMessage-object by copying
 * data from another, allready existing object.
 * 
 * \param rhs
 * Reference of the initializing object.
 */
WOscMessage::WOscMessage(const WOscMessage& rhs){
	Copy(rhs);
}
/** Copy operator.
 * Overwrites the content of a WOscMessage-object by copying
 * data from another, allready existing object.
 * 
 * \param rhs
 * Initializing object (Right Hand Side).
 * 
 * \returns
 * The reference to the initialized object (allows nesting).
 */
WOscMessage& WOscMessage::operator= (const WOscMessage& rhs){
	Copy(rhs);
	return *this;
}

/** Merges the arguments of two messages, the right hand address will be ignored.
 * Merges the right hand side (rhs) object reference into the 
 * left hand side (lhs) object and returns a reference to it.
 * The merging keeps the argument order of both messages but only the OSC-address
 * of the lhs object remains. The order of the arguments of the resulting message
 * is: First the lhs arguments in its prior order the the rhs arguments in its 
 * prior order.
 * 
 * \param rhs
 * Reference to class which arguments should be merged into this class.
 * 
 * \returns
 * A reference to the merged (this) class.
 */
WOscMessage& WOscMessage::operator+= (const WOscMessage& rhs){
	// merge messages	
	return *this;
}

/** Merges two messages but does not overwrite the left hand side
 * object.
 * The same as WOscMessage::operator+= (WOscMessage& rhs) but it does
 * not modify the lhs object.
 *
 * \param rhs
 * Reference to class which arguments should be merged into the
 * combined class.
 * 
 * \returns
 * The merged version of the lhs and rhs objects.
 *
 * \see
 * WOscMessage::operator+= (WOscMessage& rhs)
 */
WOscMessage WOscMessage::operator+ (const WOscMessage& rhs){
	WOscMessage tmp(*this);
	tmp += rhs;
	return tmp;
}

/** Copy construction by pointer. 
 * The same as WOscMessage::WOscMessage(WOscMessage& rhs) but
 * with pointer as parameter.
 * 
 * \param message
 * Pointer to the object, which should be used as copying source.
 *
 * \see
 * WOscMessage::WOscMessage(WOscMessage& rhs)
 */
WOscMessage::WOscMessage(const WOscMessage* message){
	Copy(*message);
}

/** Copy a WOscMessage-reference.
 * Used internally for copying objects.
 *
 * \param msg
 * Source.
 * 
 * \remarks
 * Used by numerous public member functions.
 *
 * \see
 * WOscMessage::WOscMessage(WOscMessage* message) |
 * WOscMessage& WOscMessage::operator= (WOscMessage& rhs) |
 * WOscMessage::WOscMessage(WOscMessage& rhs) |
 */
void WOscMessage::Copy(const WOscMessage &msg){
	
	int i;

	m_address = msg.m_address;
	m_typeTag = msg.m_typeTag;

	if ( (m_numFloats = msg.m_numFloats) > 0 ){
		m_floats = new float[m_numFloats];
		for ( i = 0; i < m_numFloats  ; i++ )
			m_floats[i] = msg.m_floats[i];
	}else
		m_floats = NULL;

	if ( (m_numInts = msg.m_numInts) > 0 ){
		m_ints = new int[m_numInts];
		for ( i = 0; i < m_numInts  ; i++ )
			m_ints[i] = msg.m_ints[i];
	}else
		m_ints = NULL;

	if ( (m_numStrings = msg.m_numStrings) > 0 ){
		m_strings = new WOscString[m_numStrings];
		for ( i = 0; i < m_numStrings  ; i++ )
			m_strings[i] = msg.m_strings[i];
	}else
		m_strings = NULL;

	if ( (m_numBlobs = msg.m_numBlobs) > 0 ){
		m_blobs = new WOscBlob*[m_numBlobs];
		for ( i = 0; i < m_numBlobs  ; i++ )
			m_blobs[i] = new WOscBlob( msg.m_blobs[i] );
	}else
		m_blobs = NULL;

	if ( (m_bufferLen = msg.m_bufferLen) > 0 ){
		m_buffer = new char[m_bufferLen];
		memcpy(m_buffer, msg.m_buffer, m_bufferLen);
	}else
		m_buffer = NULL;

}

/** Generates the binary representation of an OSC-message
 * with its arguments.
 * It first extracts the length of the resulting buffer
 * by scanning the type-tags and arguments and then
 * extracts the binary representation of the arguments and
 * merges it together with the address and type-tag.
 * 
 * \remarks
 * Used by most constructors and add-functions. Sizes in
 * OSC are always a multiple of 4.
 */
void WOscMessage::GenerateBufferFromMembers(){
	
	int i, bufWrPos, size;
	int idx_i = 0, idx_f = 0, idx_s = 0, idx_b = 0;

	// if buffer exists -> delete
	if (m_buffer)
		delete [] m_buffer;

	/* compute buffer length from type tag contents and args...*/
	/* ... */

	/* get address and type-tag length */
	int typeTagSize = m_typeTag.GetSize();
	m_bufferLen = m_address.GetSize() + typeTagSize; 

	/* get a parsable form of the type-tag (string)
	 * */	
	const char* typeTag = m_typeTag.GetBuffer(); 

	for ( i = 0; i < typeTagSize; i++ ){
		switch (typeTag[i]){
			case 'i':
				m_bufferLen += WOscUtil::OSC_INT_SIZE;
				break;
			case 'f':
				m_bufferLen += WOscUtil::OSC_FLOAT_SIZE;
				break;
			case 's':
				m_bufferLen += m_strings[idx_s++].GetSize();
				break;
			case 'b':
				m_bufferLen += m_blobs[idx_b++]->GetBufferLen();
				break;

			case ',':
				break;
			case '\0':
				i = typeTagSize; // jump to end
				break;
			default: // error
				i = typeTagSize; // jump to end
				break;
		}
	}

	// allocate buffer
	m_buffer = new char[m_bufferLen];

	/* fill buffer */
	
	// a) with osc address
	m_address.GetBuffer(m_buffer, m_address.GetSize());
	
	// b) with typetag
	m_typeTag.GetBuffer(m_buffer + m_address.GetSize(), m_typeTag.GetSize());
	bufWrPos = m_address.GetSize() + m_typeTag.GetSize();

	// c) arguments from typetagstring
	idx_i = idx_f = idx_s = idx_b = 0;
	for ( i = 0; i < typeTagSize; i++ ){
		switch (typeTag[i]){
			case 'i': // integer args
				/* write arg to buffer */
				WOscUtil::WriteBuffer(&(m_buffer[bufWrPos]), m_ints[idx_i]);
				/* increment write buffer position by argsize */
				bufWrPos += WOscUtil::OSC_INT_SIZE;
				/* increase type-specific index */
				idx_i++;
				break;
			case 'f': // float args
				WOscUtil::WriteBuffer(&(m_buffer[bufWrPos]), m_floats[idx_f]);
				bufWrPos += WOscUtil::OSC_FLOAT_SIZE;
				idx_f++;
				break;
			case 's': // string args
				size = m_strings[idx_s].GetSize();
				m_strings[idx_s].GetBuffer(&(m_buffer[bufWrPos]), size );
				bufWrPos += size;
				idx_s++;
				break;
			case 'b': // blob args
				size = m_blobs[idx_b]->GetBufferLen();
				m_blobs[idx_b]->GetBuffer(&(m_buffer[bufWrPos]), size );
				bufWrPos += size;
				idx_b++;
				break;
			case ',': // beginning of the type-tag
				/* skip it */
				break;
			case '\0': // end of the type tag
				i = typeTagSize; // jump to end
				break;
			default: // error
				i = typeTagSize; // jump to end
				break;
		}
	}
}


/** Adds a float to the current message.
 * Inserts it in the type tag and adds it to the float array.
 *
 * \param floatArg
 * Float message-argument to be added.
 * 
 * \remarks
 * Because the typetags are appended to the end of the typetag,
 * the order of the arguments can be reconstructed always.
 * 
 */
void WOscMessage::Add(float floatArg){
	
	m_typeTag += 'f';

	if ( m_floats == NULL ){
		m_floats = new float[1];
		m_floats[0] = floatArg;
		m_numFloats = 1;
	}else{
		m_numFloats++;
		float* newFloats = new float[m_numFloats];
		for( int i = 0; i < m_numFloats-1; i++ )
			newFloats[i] = m_floats[i];
		newFloats[m_numFloats-1] = floatArg;
		delete [] m_floats;
		m_floats = newFloats;
	}
	GenerateBufferFromMembers();
}

/** Adds an integer to the current message.
 * Inserts it in the type tag and adds it to the integer array.
 *
 * \param intArg
 * Integer message-argument to be added.
 * 
 * \remarks
 * Because the typetags are appended to the end of the typetag,
 * the order of the arguments can be reconstructed always.
 * 
 */
void WOscMessage::Add(int intArg){

	m_typeTag += 'i';

	if ( m_ints == NULL ){
		m_ints = new int[1];
		m_ints[0] = intArg;
		m_numInts = 1;
	}else{
		m_numInts++;
		int* newInts = new int[m_numInts];
		for( int i = 0; i < m_numInts-1; i++ )
			newInts[i] = m_ints[i];
		newInts[m_numInts-1] = intArg;
		delete [] m_ints;
		m_ints = newInts;
	}
	GenerateBufferFromMembers();
}

/** Adds a string to the current message.
 * Inserts it in the type tag and adds it to the string array.
 *
 * \param s
 * String message-argument to be added.
 * 
 * \remarks
 * Because the typetags are appended to the end of the typetag,
 * the order of the arguments can be reconstructed always.
 * 
 */
void WOscMessage::Add(const char* s){	
	m_typeTag += 's';

	if ( m_strings == NULL ){
		m_strings = new WOscString[1];
		m_strings[0] = s;
		m_numStrings = 1;
	}else{
		m_numStrings++;
		WOscString* newStrings = new WOscString[m_numStrings];
		for( int i = 0; i < m_numStrings-1; i++ )
			newStrings[i] = m_strings[i];
		newStrings[m_numStrings-1] = s;
		delete [] m_strings;
		m_strings = newStrings;
	}
	GenerateBufferFromMembers();
}

/** Adds a string to the current message.
 * Inserts it in the type tag and adds it to the string array.
 *
 * \param s
 * String message-argument to be added.
 * 
 * \throws None.
 * 
 * \remarks
 * Because the typetags are appended to the end of the typetag,
 * the order of the arguments can be reconstructed always.
 * 
 */
void WOscMessage::Add(WOscString& s){

	m_typeTag += 's';

	if ( m_strings == NULL ){
		m_strings = new WOscString[1];
		m_strings[0] = s;
		m_numStrings = 1;
	}else{
		m_numStrings++;
		WOscString* newStrings = new WOscString[m_numStrings];
		for( int i = 0; i < m_numStrings-1; i++ )
			newStrings[i] = m_strings[i];
		newStrings[m_numStrings-1] = s;
		delete [] m_strings;
		m_strings = newStrings;
	}
	GenerateBufferFromMembers();
}


/** Adds a n OSC-blob to the current message.
 * Inserts it in the type tag and adds it to the string array.
 *
 * \param b
 * Pointer to a OSC-blob message-argument to be added.
 * 
 * \remarks
 * Because the typetags are appended to the end of the typetag,
 * the order of the arguments can be reconstructed always.
 * perhaps in later versions only the references of a blob are 
 * 
 * \todo: take ownership of blob or do reference counting instead
 * of allocating multiple times.
 */
void WOscMessage::Add(WOscBlob* b){
	m_typeTag += 'b';

	if ( m_blobs ){
		m_numBlobs++;
		WOscBlob** newBlobs = new WOscBlob*[m_numStrings];
		for( int i = 0; i < m_numBlobs-1; i++ )
			newBlobs[i] = m_blobs[i]; // copy old pointers
		newBlobs[m_numBlobs-1] = new WOscBlob(b); // add new element
		delete [] m_blobs; // delete old array
		m_blobs = newBlobs; // assign new array
	}else{
		m_blobs = new WOscBlob*[1];
		m_blobs[0] = new WOscBlob(b);
		m_numBlobs++;
	}
	GenerateBufferFromMembers();
}

/** Returns the number of floating-point arguments contained in this message.
 * Use it for accessing arguments without generating out-of-bound-exceptions
 * ( take a look at WOscException::ErrorCodes and the corresponding 
 * ERR_INVALID_INDEX code).
 *
 * \returns
 * The number of floating-point arguments contained in this message.
 * 
 */
int WOscMessage::GetNumFloats() const {
	return m_numFloats;
}

/** Returns the number of integer arguments contained in this message.
 * Use it for accessing arguments without generating out-of-bound-exceptions
 * ( take a look at WOscException::ErrorCodes and the corresponding 
 * ERR_INVALID_INDEX code).
 *
 * \returns
 * The number of integer arguments condained in this message.
 * 
 */
int WOscMessage::GetNumInts() const {
	return m_numInts;
}

/** Returns the number of string arguments contained in this message.
 * Use it for accessing arguments without generating out-of-bound-exceptions
 * ( take a look at WOscException::ErrorCodes and the corresponding 
 * ERR_INVALID_INDEX code).
 *
 * \returns
 * The number of string arguments contained in this message.
 * 
 */
int WOscMessage::GetNumStrings() const {
	return m_numStrings;
}

/** Returns the number of OSC-blob arguments contained in this message.
 * Use it for accessing arguments without generating out-of-bound-exceptions
 * ( take a look at WOscException::ErrorCodes and the corresponding 
 * ERR_INVALID_INDEX code).
 *
 * \returns
 * The number of OSC-blob arguments contained in this message.
 */
int WOscMessage::GetNumBlobs() const {
	return m_numBlobs;
}



/** Returns the float argument with the index 'idx'.
 * Argument data is kept internally in array data-structures. 
 * This function is an safe interface.
 *
 * \param idx
 * Index of float argument contained in the message.
 * 
 * \returns
 * The float with index "idx".
 * 
 * \throws WOscException
 * When idx is bigger than the number of floats contained
 * in the message an exception will be thrown.
 * 
 * \see
 * WOscException::ErrorCodes
 */
float WOscMessage::GetFloat(int idx) const {
	if ( idx > m_numFloats )
		throw new WOscException(
			WOscException::ERR_INVALID_INDEX,
			"WOscMessage: Index out of range when trying to"
			"get an float message-argument.");
	return m_floats[idx];
}

/** Returns the integer argument with the index 'idx'.
 * Argument data is kept internally in array data-structures. 
 * This function is an safe interface.
 *
 * \param idx
 * Index of integer argument contained in the message.
 * 
 * \returns
 * The integer with index "idx".
 * 
 * \throws WOscException
 * When idx is bigger than the number of integers contained
 * in the message an exception will be thrown.
 * 
 * \see
 * WOscException::ErrorCodes
 */
int WOscMessage::GetInt(int idx) const {
	if ( idx > m_numInts )
		throw new WOscException(
			WOscException::ERR_INVALID_INDEX,
			"WOscMessage: Index out of range when trying to"
			"get an int message-argument.");
	return m_ints[idx];
}

/** Returns the WOscString argument with the index 'idx'.
 * Argument data is kept internally in array data-structures. 
 * This function is an safe interface.
 *
 * \param idx
 * Index of the WOscString-argument contained in the message.
 * 
 * \returns
 * The WOscString-argument with index "idx".
 * 
 * \throws WOscException
 * When idx is bigger than the number of WOscString-arguments
 * contained in the message an exception will be thrown.
 * 
 * \see
 * WOscException::ErrorCodes | WOscString
 */
WOscString& WOscMessage::GetString(int idx) const {
	if ( idx > m_numStrings )
		throw new WOscException(
			WOscException::ERR_INVALID_INDEX,
			"WOscMessage: Index out of range when trying to"
			"get an string message-argument.");
	return m_strings[idx];
}

/** Returns a pointer to the WOscBlob argument with the index 'idx'.
 * Argument data is kept internally in array data-structures. 
 * This function is an safe interface.
 *
 * \param idx
 * Index of the WOscBlob-argument contained in the message.
 * 
 * \returns
 * The WOscBlob-argument with index "idx".
 * 
 * \throws WOscException
 * When idx is bigger than the number of WOscBlob-arguments
 * contained in the message an exception will be thrown.
 * 
 * \see
 * WOscException::ErrorCodes | WOscBlob
 */
WOscBlob* WOscMessage::GetBlob(int idx) const {
	if ( idx > m_numBlobs )
		throw new WOscException(
			WOscException::ERR_INVALID_INDEX,
			"WOscMessage: Index out of range when trying to"
			"get an blob message-argument.");
	return m_blobs[idx];
}

/** Fills the buffer with the raw bytestream of this packet.
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
 * when modifying the message-object afterwards. (contrary to
 * WOscMessage::getBuffer() )
 * 
 * \see
 * WOscMessage::getBuffer()
 */
void WOscMessage::GetBuffer(char* buffer, int bufferLen) {
	if ( bufferLen < m_bufferLen ){
		throw new WOscException(
			WOscException::ERR_OSC_STR_BUFF_TOO_SMALL,
			"WOscMessage: Buffer too small when trying to copy string to buffer.");
	}else{
		for ( int i = 0; i < bufferLen; i++ )
			buffer[i] = m_buffer[i];
	}
}

/** Returns a pointer to an internal buffer containing
 * the current binary representation of this message.
 * Please See remarks for safety issues.
 * 
 * \returns
 * Pointer to internal array data.
 * 
 * \remarks
 * The pointer remains valid until the modification of this message object.
 * The next time an argument will be added to the message, the message is 
 * destination of an operation (check operators), the message will be deleted
 * or one of the getBuffer functions is called this pointer gets invalid. If you are
 * not sure about that issue, please use the other getBuffer()- function.
 * 
 * \see
 * WOscMessage::getBuffer(char* buffer, int bufferLen)
 */
const char* WOscMessage::GetBuffer(){
	return m_buffer;
}

/** Returns the size of the binary message representation.
 * Can be used to allocate memory for instance.
 * 
 * \returns
 * Size of the internal binary buffer.
 * 
 * \remarks
 * Remains valid: see remarks of WOscMessage::getBuffer().
 */
int WOscMessage::GetBufferLen() {
	return m_bufferLen;
}


/** Returns the OSC address of this message.
 * 
 * \returns
 * OSC string containing the OSC address.
 * 
 */
WOscString WOscMessage::GetOscAddress() const {
	return m_address;
}
