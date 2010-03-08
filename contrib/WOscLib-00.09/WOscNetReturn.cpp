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

/** WOscNetReturn source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-03-20 19:41:37 $
 * $Revision: 1.2 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscNetReturn.h"


/** Constructor.
 * Resets the parent counter (no parents yet).
 * 
 */
WOscNetReturn::WOscNetReturn(){
	m_numUsers = 0;
}

/** An owner (user) of this object increases the parent counter
 * to request an ongoing existence.
 * To unregister call WOscNetReturn::removeParent() .
 * 
 * \see
 * WOscNetReturn::removeParent()
 */
void WOscNetReturn::AddParent(){
	m_numUsers++;
}
/** An owner (user) of this object decreases the parent counter
 * to announce, that it isn't interested in this object anymore.
 * If the parent counter reaches zero, the object destructs itself.
 * 
 * \see
 * WOscNetReturn::addParent()
 */
void WOscNetReturn::RemoveParent(){
	m_numUsers--;
	if ( m_numUsers <= 0 )
		delete this;
}

/** Destructor.
 *
 */
WOscNetReturn::~WOscNetReturn(){
}

