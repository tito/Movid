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

/** WOscReceiver source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2007-11-30 00:27:08 $
 * $Revision: 1.6 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#include "WOscReceiver.h"



 #if WOSC_USE_PRIORITY_QUEUE == 1
/** Constructor.
 * Constructs a receiver and initilizes its priority-
 * queue.
 * 
 * \param initialQueueCapacity
 * Optional size of priority queue. Grows automatically quadratically if
 * too small.
 * 
 * \param systemTime
 * Optional system-time object (inherited class from WOscSystemTime) which
 * supplies the receiver with system dependent time information. Passed
 * objects will be deleted by the receiver object destructor.
 */
WOscReceiver::WOscReceiver(int initialQueueCapacity /* = DEFAULT_QUEUE_CAPACITY*/, WOscSystemTime* systemTime /*= NULL*/)
/* init base class */
:WOscPriorityQueue(initialQueueCapacity)
#else
/** Constructor.
 * Constructs a receiver and initilizes its priority-
 * queue.
 * 
 * \param systemTime
 * Optional system-time object (inherited class from WOscSystemTime) which
 * supplies the receiver with system dependent time information. Passed
 * objects will be deleted by the receiver object destructor.
 */
WOscReceiver::WOscReceiver(WOscSystemTime* systemTime /*= NULL*/)
#endif	//  #if WOSC_USE_PRIORITY_QUEUE == 1
/* init inherited class */
{
	/* reset address space */
	m_addrSpaceRootContainer = NULL;

	/* set system time generator to NULL when not specified*/
	m_systemTime = systemTime;
}

/** Cleans up the receiver.
 * All elements which are not removed from the priority-queue
 * get deleted automatically.
 */
WOscReceiver::~WOscReceiver(){
	if ( m_systemTime ) 
		delete m_systemTime;
}

/** Sets the address-space of the WOscReceiver.
 * The address-space can be modified dynamically and externally, 
 * desired, if the root-container remains valid.
 * 
 * \param addrSpaceRootContainer
 * Root-container. Can not be a child.
 * 
 * \throws WOscException
 * If root-container is not root.
 * 
 * \remarks
 * If currently no address-space should be available, pass NULL. 
 * This disables the invokation of messages.
 */
void WOscReceiver::SetAddressSpace(WOscContainer* addrSpaceRootContainer){
	if ( ! addrSpaceRootContainer->IsRoot() )
		throw new WOscException(
			WOscException::ERR_NOT_ROOT_CONTAINER, 
			"Trying to set a child-container as address-space."
			"Address-space must be a root-conatiner.");
	m_addrSpaceRootContainer = addrSpaceRootContainer;
}


/** Returns the internal address-space reference.
 * 
 * \returns
 * Internal address-space, if there is none NULL.
 */
WOscContainer* WOscReceiver::GetAddressSpace(){
	return m_addrSpaceRootContainer;
}

/** Parses a received packet, constructs messages and/or bundles of it
 * and passes it to the invocation-engine, which scans the
 * address-space for matching methods, takes care of time-tags
 * and queues bundles, which have to be delayed, in the
 * priority-queue.
 * 
 * \param data
 * Binary OSC-packet data.
 * 
 * \param dataLen
 * Length of binary OSC-packet data.
 * 
 * \param networkReturnAddress
 * Network packet origin.
 * 
 * \throws
 * None, but if an internal exception occurs, the user-defined
 * exception-handler is called, see below.
 * 
 * \see
 * handleOffendingPackets(const char* const data, int dataLen, WOscNetReturn networkReturnAddress, WOscException* exception)
 */
void WOscReceiver::NetworkReceive(const char* const data, int dataLen, WOscNetReturn* networkReturnAddress){

	/* check data length to not read wrong data 
	 * when guessing OSC-object
	 */
	if ( dataLen < 1 )
		return;

	/* guess if bundle or message.
	 * if bundle, the return address gets managed by the bundle
	 * and priority-queue (.
	 * if not, the return-address has to be deleted manually after
	 * the message has been processed.
	 */
	if ( data[0] == '#' ){
		
		/* could be a bundle...
		 * try to construct a bundle from it. if it should fail,
		 * the global exception handler gets called.
		 */
		WOscBundle* rxBundle = NULL;

		try{
			/* try to construct it */
			rxBundle = new WOscBundle(data, dataLen, networkReturnAddress);

		}catch(WOscException* exception){
			/* error during bundle construction */

			/* clean up (no bundle constructed -> netreturn has to be deleted manually) */
			networkReturnAddress->RemoveParent();

			/* call user defined exception handler */
			HandleOffendingPackets(data, dataLen, exception);

			/* exception handled -> remove exception */
			exception->Destroy();
			return;
		}
		try{
			/* process it */
			ProcessBundle(rxBundle);

		}catch(WOscException* exception){
			/* error during bundle processing */

			/* clean up */
			if (rxBundle)
				delete rxBundle;

			/* call user defined exception handler */
			HandleOffendingPackets(data, dataLen, exception);
			
			/* exception handled -> remove exception */
			exception->Destroy();
			return;
		}

	}else{
		/* message -> invoke*/
		WOscMessage* rxMsg = NULL;
		try{
			/* try to construct */
			rxMsg = new WOscMessage(data, dataLen);
		
		}catch(WOscException* exception){

			/* call user defined exception handler */
			HandleOffendingPackets(data, dataLen, exception);

			/* clean up (no message constructed -> netreturn has to be deleted manually) */
			networkReturnAddress->RemoveParent();
			
			/* exception handled -> remove exception */
			exception->Destroy();
			
			return;
		}
		try{
			/* process it */
			ProcessMessage(rxMsg, networkReturnAddress);

		}catch(WOscException* exception){
			/* clean up, if necessary */
			if (rxMsg)
				delete rxMsg;
			
			/* call user defined exception handler */
			HandleOffendingPackets(data, dataLen, exception);
			
			/* exception handled -> remove exception */
			exception->Destroy();
		}

		/* clean up return address.
		 * if received OSC-object is not a bundle the network-return address 
		 * is not managed by it and has to be deleted manually.
		 */
		networkReturnAddress->RemoveParent();
	}

}

/** Internal bundle processing.
 * Checks time-tag of bundle, if not ready it is queued in
 * the priority queue. Else it is unbundled, the contained
 * messages are invoked and internal bundles are passed to
 * to processBundle(WOscBundle* bundle) again (recursion).
 *
 * \param bundle
 * The bundle which has to be processed.
 * 
 * \throws WOscException
 * Bundle internal exceptions, there should be none.
 * Internal exceptions point to library bugs.
 * 
 * \remarks
 * The bundle passed to this function will be deleted when
 * processed.
 */
void WOscReceiver::ProcessBundle(WOscBundle* bundle){

#if WOSC_USE_PRIORITY_QUEUE == 1
	/* check time-tag */
	WOscTimeTag currentTime = WOscTimeTag::GetCurrentTime(m_systemTime);
	if ( *bundle <= currentTime ){
#endif
		/* it's seems to be time to process this bundle.
			* process bundle
			*/
		/* invoke all messages of current bundle */
		while ( bundle->GetNumMessages() > 0 )
			/* remove first message from bundle and process it
			 * (gets deleted afterwards).
			 */
			ProcessMessage(bundle->GetMessage(0), bundle->GetNetworkReturnAddress());

		/* process all subbundles (recursively) */
		while ( bundle->GetNumBundles() > 0 )
			/* remove first subbundle from bundle and process it
			 * (gets deleted afterwards, see below).
			 */
			ProcessBundle(bundle->GetBundle(0));

		/* bundle done, delete it */
		delete bundle;
		
#if WOSC_USE_PRIORITY_QUEUE == 1
	}else{
		/* the bundle is not ready, queue it in the priority-queue. */
		InsertItem( (WOscQueueItem*) bundle );
	}
#endif
}
/** Internal message processing.
 * The message is passed to the address-space and a
 * call-back-list is created and invoked.
 * 
 * \param msg
 * Message to be processed.
 * 
 * \param networkReturnAddress
 * The network-origin of this message.
 * 
 * \throws WOscException
 * Message internal exceptions, there should be none.
 * Internal exceptions point to library bugs.
 * 
 * \remarks
 * The message passed to this function will be deleted when
 * processed.
 */
void WOscReceiver::ProcessMessage(WOscMessage* msg, const WOscNetReturn* networkReturnAddress){
	
	WOscTimeTag currentTime = WOscTimeTag::GetCurrentTime(m_systemTime);

		if ( m_addrSpaceRootContainer ){
			/* get matching methods */
			WOscCallbackList* callBackList = m_addrSpaceRootContainer->DispatchMessage(msg);
			// check if matching methods found
			if ( callBackList->GetNumMethods() <= 0 )
				// if not, handle message in special handler
				HandleNonmatchedMessages(msg, networkReturnAddress);
			else
				// invoke them
				callBackList->InvokeAll(msg, currentTime, networkReturnAddress);
			/* clean up callback list*/
			delete callBackList;
		}
		/* message done, clean up message */
		delete msg;
}

#if WOSC_USE_PRIORITY_QUEUE == 1
/** Keep the internal priority-queue running.
 * If there are any ready-to-process-elements, 
 * they are removed from the priority-queue and
 * processed.
 * 
 * \throws WOscException
 * Bundle or message internal exceptions, there should be none.
 * Internal exceptions point to library bugs.
 * 
 * \remarks
 * Call this method perodically to keep the priority-queue running.
 */
void WOscReceiver::ManagePriorityQueue(){

	WOscTimeTag currentTime = WOscTimeTag::GetCurrentTime(m_systemTime);
	WOscBundle* readyBundle;

	/* run as long there is something ready */
	while (	(GetEarliestTimeTag() < currentTime ) &&
			(GetNumItems() > 0) )
	{
		readyBundle = NULL;	
		try{
			/* get ready element from queue */
			readyBundle = (WOscBundle*) RemoveEarliestItem();
			/* process it */
			ProcessBundle(readyBundle);

		}catch(WOscException* exception){

			if (readyBundle)
				delete readyBundle;

			/* call user defined exception handler */
			HandleOffendingPackets(NULL, 0, exception);

			/* exception handled -> remove exception */
			exception->Destroy();
		}
	}
}

#endif	// #if WOSC_USE_PRIORITY_QUEUE == 1
