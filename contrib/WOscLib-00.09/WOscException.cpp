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

/** WOscException source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscException.h"
#include <string.h>

/**  The Constructor.
 * Copies the error-string and the error-number.
 *
 * \param errNum
 * Error identifier.
 * 
 * \param errDscr
 * String with error description for users.
 * 
 * \remarks
 * When a WOscException is thown by "throw new WOscException(...) 
 * the exception must be deleted in the handler 
 * (catch(WOscException *e)) by "e->destroy()".
 * 
 */
WOscException::WOscException(int errNum, const char* errDscr){
	m_errNum = errNum;

	if ( errDscr != NULL ){
		m_errDscr = new char[ strlen(errDscr) + 1 ];
		strcpy(m_errDscr, errDscr);
	}else
		m_errDscr = NULL;
};

/** Destructor.
 * Deletes the error description string.
 */
WOscException::~WOscException(){
	if ( m_errDscr )
		delete [] m_errDscr;
}

/** Self destruction.
 * Let the object remove itself from the heap.
 * 
 * \remarks
 * When a WOscException is thown by "throw new WOscException(...) 
 * the exception must be deleted in the handler 
 * (chatch(WOscException *e)) using "e->destroy()".
 * 
 */
void WOscException::Destroy(){
	delete this;
};

/** Returns the error description as string.
 * Valid until the object is deleted.
 *
 * \returns
 * Pointer to the array with the error-description.
 */
const char* WOscException::GetDescription(){
	/*int len = (int)strlen(m_errDscr)+1;
	char* descr = new char[len];
	memcpy(descr, m_errDscr, len);
	return descr;*/
	return m_errDscr;
}

/** Returns the error code.
 * The error-code is an idientifier for the
 * exceptional situation that ocurred. Decisions
 * for handling those situations can be made
 * according to the error-code.
 * 
 * \returns
 * The error cope.
 *
 * \see
 * See ErrorCodes for a detailed description of errors.
 */
int WOscException::GetErrorCode(){
	return m_errNum;
}
