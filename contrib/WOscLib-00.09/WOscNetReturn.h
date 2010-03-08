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

/** WOscNetReturn header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-12 20:02:28 $
 * $Revision: 1.4 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

#ifndef __WOSCNETRETURN_H__
#define __WOSCNETRETURN_H__

#include "WOscLib.h"

/** System independent network-address base-class. Required to
 * pass a return address of the client which send the message
 * to the method handling the message. Inherit a new class from 
 * this base class and your system-specific network address-class.
 * 
 * All address-object-references passed to networkReceive of WOscReceiver
 * (and inherited classes) will be managed and deleted internally.
 *
 * \remarks
 * Due to the priority queue and nested OSC-bundles which are inserted into it,
 * estimations about the lifetime of an address-object are difficult to make.
 * Each OSC-bundle received from a network (or similar) interface and its inner
 * bundles have the same return-address. but since inner bundles (bundles in bundles)
 * can have larger time-tags than the outer bundle, the return address has to be
 * preserved. This would make a memory-management on bundle-level complicated.
 * This library delegates this task to a lower level: To the elements, which get
 * queued in a priority queue (i.e. bundles) and a special return-address base-class.
 *
 * WOscNetReturn features a garbage-collector like memory-management which keeps the
 * object alive as long there are parents. Each parent has to register and unregister
 * itself by calling addParent() and removeParent() resp..
 *
 * If the last object unregisters itself, the object gets deleted.
 *
 * In this library the WOscQueueItem and WOscReceiver are the only users of objects
 * of this kind.
 *
 * All return-address management in this library is handled through references, thus
 * the library does not have to be recompiled or even changed when using external 
  *internet-(or similar for different transport layers) addresses. Simply inherit.
 * 
 * \see
 * WOscReceiver::networkReceive(const char* const data, int dataLen, WOscNetReturn* networkReturnAddress) , 
 * WOscQueueItem::WOscQueueItem(WOscTimeTag& timeTag, WOscNetReturn* ra) , 
 * WOscQueueItem::~WOscQueueItem() and the examples.
 */
class WOSC_EXPORT WOscNetReturn{

protected:
	WOscNetReturn();
	virtual ~WOscNetReturn();

	friend class WOscQueueItem;	/**< The parent object (WOscQueueItem) determines if a network-return
								 * address is an orphan by adding and removing itself as a parent.
								 */
	friend class WOscReceiver;	/**< When receiving a message, the network-return-address has to be 
								 * deleted in receiver.
								 */
	void AddParent();
	void RemoveParent();

private:
	int  m_numUsers;	/**< Number of WOscQueueItems are using this element currently.*/

	/**
	 * \example WOscServer.cpp
	 * An example of how to inherit WOscNetReturn is shown here.
	 * The header (WOscServer.h) is prepended here:
	 * \include WOscServer.h
	 */

};

#endif
