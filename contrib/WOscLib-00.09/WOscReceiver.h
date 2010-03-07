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

/** WOscReceiver header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-08-12 20:02:28 $
 * $Revision: 1.6 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCRECEIVER_H__
#define __WOSCRECEIVER_H__


#include "WOscContainer.h"
#include "WOscPriorityQueue.h"
#include "WOscException.h"
#include "WOscBundle.h"
#include "WOscLib.h"

/** An OSC-receiving-system.
 * The WOscReceiver-class is a complete system for processing
 * incoming OSC-data (i.e. bundles or messages). It queues
 * bundles, which have to be delayed because of their time-tags,
 * unbundles bundles, passes the correct return addresses to the
 * methods or errorhandlers.
 *
 * The address space can be set dynamically,
 * new methods/containers can be added/removed.
 *
 * The receiver can be wrapped into a thread easily:
 * Poll periodically for network packets while calling the
 * priority-queue management function ( managePriorityQueue() ).
 * When receiving a network packet, pass it to the
 * networkReceive(char* data, int dataLen, WOscNetReturn *networkReturnAddress)
 * function.
 *
 * \remarks
 * Override the networkSend(char* data, int dataLen, WOscNetReturn *networkReturnAddress)
 * function for providing a network return-path and a handler
 * for defective packets,
 * handleOffendingPackets(char* data, int dataLen, WOscNetReturn *networkReturnAddress)
 * 
 * \see
 * None.
 */
 
#if WOSC_USE_PRIORITY_QUEUE == 1
class WOscReceiver: private WOscPriorityQueue{
#else
class WOscReceiver{
#endif	// #if WOSC_USE_PRIORITY_QUEUE == 1

public:
#if WOSC_USE_PRIORITY_QUEUE == 1
	WOscReceiver(int initialQueueCapacity = DEFAULT_QUEUE_CAPACITY, WOscSystemTime* systemTime = NULL);
#else
	WOscReceiver(WOscSystemTime* systemTime = NULL);
#endif	// #if WOSC_USE_PRIORITY_QUEUE == 1

	virtual ~WOscReceiver();

	void SetAddressSpace(WOscContainer* addrSpaceRootContainer);
	WOscContainer* GetAddressSpace();

	void NetworkReceive(const char* const data, int dataLen, WOscNetReturn* networkReturnAddress);
#if WOSC_USE_PRIORITY_QUEUE == 1
	void ManagePriorityQueue();
#endif


	/** Virtual function which has to be overridden by the user.
	 * It then can be used to send OSC messages from within WOscReceiverMethod
	 * message callbacks. It is the interface to the network layer for sending
	 * messages.
	 * 
	 * \param data
	 * Pointer to buffer containning the data to be sent.
	 * 
	 * \param dataLen
	 * Length of the data in the buffer.
	 * 
	 * \param networkReturnAddress
	 * Destination network address for the data.
	 * 
	 */
	virtual void NetworkSend(const char* data, int dataLen, const WOscNetReturn* networkReturnAddress) = 0;

	/** Constants related to a WOscReceiver.
	 */
	enum Constants{
		DEFAULT_QUEUE_CAPACITY = 32,	/**< Default capacity of the priority queue.*/
	};

protected:

	/** Handler for packets which caused either an internal exception or another
	 * error. The user can decide what to do with such packets by 
	 * overriding this function.
	 * 
	 * \param data
	 * Pointer to the buffer containing the offending packet data.
	 * 
	 * \param dataLen
	 * Lenght of the buffer containg the offending packet data.
	 * 
	 * \param exception
	 * Exception which has been generated during processing this packet.
	 * 
	 */
	virtual void HandleOffendingPackets(const char* const data, int dataLen, WOscException* exception) = 0;
	/** Handler for OSC messages which didn't match any address in the OSC 
	 * address space. The user can define an own action by overriding this 
	 * pure virtual function.
	 * 
	 * \param msg
	 * Pointer to Message which hasn't been processed.
	 * 
	 * \param networkReturnAddress
	 * Network address of the sender of this message.
	 * 
	 */
	virtual void HandleNonmatchedMessages(const WOscMessage* msg, const WOscNetReturn* networkReturnAddress) = 0;

private:
	void ProcessBundle(WOscBundle* bundle);
	void ProcessMessage(WOscMessage* msg, const WOscNetReturn* networkReturnAddress);

	WOscContainer*		m_addrSpaceRootContainer;	/**< The address space root container.*/
	WOscSystemTime*		m_systemTime;				/**< The system-time generator. */

	/** An example of how to use WOscReceiver.
	 * \example WOscServer.cpp
	 * An example of how to use WOscReceiver is shown here.
	 * The header (WOscServer.h) prepended:
	 * \include WOscServer.h
	 */
};

#endif	// #ifndef __WOSCRECEIVER_H__
