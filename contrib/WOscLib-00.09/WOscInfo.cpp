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

/** WOscInfo source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-03-19 12:14:55 $
 * $Revision: 1.1 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscInfo.h"
#include <string.h>

/** Default constructor.
 * Creates an empty description.
 * 
 * \see
 * WOscInfo::WOscInfo(const char* description)
 */
WOscInfo::WOscInfo(){
	m_description = NULL;
}

/** Constructs an info containing a description (string).
 * 
 * \param description
 * String conatianing the description of the object.
 */
WOscInfo::WOscInfo(const char* description){
	m_description = new char[strlen(description) + 1];
	strcpy(m_description, description);
}

/** Copy constructor.
 * Copies the internal data from the reference into the new
 * object.
 * 
 * \param info
 * Reference of object to be copied.
 */
WOscInfo::WOscInfo(WOscInfo& info){
	if ( (&info)->m_description ){
		m_description = new char[strlen((&info)->m_description) + 1];
		strcpy(m_description, info.m_description);
	}else
		m_description = NULL;
}

/** Destructor.
 * If internal data it will be freed.
 */
WOscInfo::~WOscInfo(){
	if( m_description )
		delete [] m_description;
}

