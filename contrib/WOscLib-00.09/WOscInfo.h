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

/** WOscInfo header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-05-16 21:21:41 $
 * $Revision: 1.2 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCINFO_H__
#define __WOSCINFO_H__

#include "WOscLib.h"

/** Information class for address-space elements (i.e. containers and methods).
 * Up to now it contains a string with the description of the associated element.
 * 
 * \see
 * WOscMethodInfo | WOscContainerInfo
 */
class WOSC_EXPORT WOscInfo{
public:
	WOscInfo();
	WOscInfo(WOscInfo& info);
	WOscInfo(const char* description);

	virtual ~WOscInfo();
	
	//virtual WOscInfo& operator= (WOscInfo& rhs);
private:
	char* m_description;	/**< String with description of the associated element.*/
};

#endif	// #ifndef __WOSCINFO_H__
