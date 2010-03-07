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

/** WOscMethod , WOscCallbackList and WOscCallbackListItem header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.4 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

#ifndef __WOSCRECEIVERMETHOD_H__
#define __WOSCRECEIVERMETHOD_H__


#include "WOscMethod.h"
#include "WOscReceiver.h"


/** Adds functionality to WOscMethod required that it can be used in WOscReceiver.
 * This happens by handling the internal context pointer in a type-save way by
 * providing constructors and functions.
 * 
 */
class WOscReceiverMethod: public WOscMethod{
public:
	WOscReceiverMethod(WOscContainer* parent, WOscReceiver* receiverContext,
        const char* methodName, const char* methodDescription);

	virtual ~WOscReceiverMethod();

protected:
	WOscReceiver* GetContext();

	/** An example of how to inherit from WOscReceiverMethod.
	 * \example WOscServer.cpp
	 * The header (WOscServer.h) prepended:
	 * \include WOscServer.h
	 */
};

#endif	// #ifndef __WOSCRECEIVERMETHOD_H__
