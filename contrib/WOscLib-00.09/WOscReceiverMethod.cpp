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

/** WOscReceiverMethod source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

#include "WOscReceiverMethod.h"

/** Contructor of an OSC receiver method.
 * 
 * \param parent
 * Pointer to parent container.
 * 
 * \param receiverContext
 * Pointer to the receiver owning this method. The context can be used later in receiver
 * methods (i.e. for accessing OSC receiver functionality inside OSC methods).
 * 
 * \param methodName
 * Address pattern (e.g. "echo").
 * 
 * \param methodDescription
 * Description of this method.
 */
WOscReceiverMethod::WOscReceiverMethod(WOscContainer* parent, WOscReceiver* receiverContext, const char* methodName, const char* methodDescription):
WOscMethod(new WOscMethodInfo(methodDescription), parent, receiverContext, methodName)
{
}


/** Destructor.
 * Removes the method info from this method (allocated by the constructor).
 */
WOscReceiverMethod::~WOscReceiverMethod(){
	delete m_info;
}

/** Returns a pointer to the OSC receiver owning this method.
 * Casts the internal context-pointer to OSC receiver.
 * 
 * \returns
 * The receiver context of the OSC method.
 */
WOscReceiver* WOscReceiverMethod::GetContext(){
	return (WOscReceiver*)m_context;
}

