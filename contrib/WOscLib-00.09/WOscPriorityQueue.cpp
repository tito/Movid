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

/** WOscQueueItem and WOscPriorityQueue source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-03-20 20:31:47 $
 * $Revision: 1.3 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */


#include "WOscPriorityQueue.h"
#include "WOscException.h"

/*  -----------------------------------------------------------------------  */
/* 	WOscQueueItem                                                            */
/*  -----------------------------------------------------------------------  */

/** Constructor for received items.
 * Received items have a network return address and a time-tag.
 * The network return address has to be registered to keep it alive
 * for all nested items.
 * 
 * \param timeTag
 * Time tag of the item.
 * 
 * \param ra
 * Network return address of the item.
 * 
 * \see
 * WOscNetReturn
 */
WOscQueueItem::WOscQueueItem(const WOscTimeTag& timeTag, WOscNetReturn* ra):
WOscTimeTag(timeTag)
{
	/* copy reference */
	m_ra = ra;
	/* register myself, since i'm using it */
	m_ra->AddParent();
}

/** Constructor for items to be transmitted.
 * Items destined for transmission do not have a return address.
 * 
 * \param timeTag
 * Time-tag of the item.
 * 
 */
WOscQueueItem::WOscQueueItem(const WOscTimeTag& timeTag):
WOscTimeTag(timeTag)
{
	/* not a received packet. for transmitted packets
	 * a return address doesn't exist */
	m_ra = NULL;
}

/** Default constructor for items to be transmitted.
 * The time-tag will be set according to the
 * time-tag default constructor (See below).
 * 
 * 
 * \see
 * WOscTimeTag
 */
WOscQueueItem::WOscQueueItem():
WOscTimeTag()
{
	/* not a received packet. for transmitted packets
	 * a return address doesn't exist */
	m_ra = NULL;
}

/** Destructor.
 * If it should be a received item, it unregisters itself from 
 * the network return address (depends on constructor).
 * 
 * \see
 * WOscNetReturn
 */
WOscQueueItem::~WOscQueueItem(){
	/* if it's a received packet, it should have a
	 * return address. unregister it before death
	 * of this object.
	 */
	if ( m_ra )
		m_ra->RemoveParent();
}

// 
/** Returns the network return-address.
 * 
 * \returns
 * A pointer to the network return-address, when it is a received
 * item. When not, it will be NULL.
 * 
 */
const WOscNetReturn* WOscQueueItem::GetNetworkReturnAddress(){
	return m_ra;
}

/*  -----------------------------------------------------------------------  */
/* 	WOscPriorityQueue                                                        */
/*  -----------------------------------------------------------------------  */
#if WOSC_USE_PRIORITY_QUEUE == 1
/** Constructor.
 * Constructs an empty priority queue with a capacity of
 * "initialCapacity".
 * 
 * \param initialCapacity
 * The initial capacity (how many items can be inserted until
 * doubleCapacity() will be called.
 * 
 * \see
 * WOscPriorityQueue::doubleCapacity()
 */
WOscPriorityQueue::WOscPriorityQueue(int initialCapacity){

	/* allocate list (queue) */
	m_listCapacity = initialCapacity;
	m_list = new WOscQueueItem*[initialCapacity];

	/* reset list (no items) */
	for( int i = 0; i < initialCapacity; i++ )
		m_list[i] = NULL;
	m_itemsInList = 0;

}

/** Destructor.
 * All elements which are not removed from the priority-queue
 * get deleted automatically.
 * 
 */
WOscPriorityQueue::~WOscPriorityQueue(){

	/* empty queue as long there are items in the queue*/
	while( GetNumItems() > 0 )
		delete RemoveEarliestItem();

	delete [] m_list;
}

/** Inserts an item into the queue.
 * Items are simply inserted and not ordered. 
 * 
 * \remarks
 * WOscPriorityQueue::getEarliestTimeTag() then checks
 * the time-tags and returns the earliest.
 * 
 */
void WOscPriorityQueue::InsertItem(WOscQueueItem *item){

	// check capacity, double if necessary
	if ( m_itemsInList == m_listCapacity )
		DoubleCapacity();

	// insert pointer to element and increment population
	m_list[m_itemsInList++] = item;

}

/** Doubles the capacity of the queue.
 * If a new item is about to be inserted into the queue and the
 * capacity is exhausted, the insertion function doubles
 * the capacity.
 */
void WOscPriorityQueue::DoubleCapacity(){
	// allocate larger list
	int newListCapacity = 2 * m_listCapacity;
	WOscQueueItem** newList = new WOscQueueItem*[newListCapacity];

	// copy old elements
	int i = 0;
	for ( ; i < m_listCapacity; i++ )
		newList[i] = m_list[i];

	// init new elements
	for ( ; i < newListCapacity; i++ )
		newList[i] = NULL;

	// activate new list and delete old
	delete [] m_list;
	m_list = newList;
	m_listCapacity = newListCapacity;
}

/** Returns the timetag of the earliest item.
 * Searches by linearly traversing the queue.
 * 
 * \returns
 * The time-tag of the earliest item. If no time-tag smaller
 * than WOscTimeTag::getLargestTimeTag() can be found
 * WOscTimeTag::getLargestTimeTag() will be returned.
 */
WOscTimeTag WOscPriorityQueue::GetEarliestTimeTag(){
	
	WOscTimeTag smallest = WOscTimeTag::GetLargestTimeTag();

	for ( int i = 0; i < m_itemsInList; i++ )
		if ( *m_list[i] < smallest )
			smallest = *m_list[i];

	return smallest;
}

/** Remove item with index idx from the queue.
 * Decrements the number of items contained in the queue.
 * 
 * \param idx
 * Index of the item to be removed.
 * 
 */
void WOscPriorityQueue::RemoveItem(int idx){
	
	m_itemsInList--;

	for ( int i = idx; i < m_itemsInList; i++ )
		m_list[i] = m_list[i+1];
}

/** Removes the earliest item from the queue and returns it.
 * 
 * \returns
 * The pointer to the earlist item removed from the queue.
 * 
 * \throws WOscException
 * When trying to remove from an empty queue.
 * 
 * \todo
 * Handle exceptions in WOscReceiver when removing items
 * from the priority queue.
 * 
 * \remarks
 * When removed, the user (caller) has to care about the
 * deletion of that element, since the queue is only an itermediate
 * buffer. When removed the queue does not know anything about this 
 * object anymore.
 *
 */
WOscQueueItem* WOscPriorityQueue::RemoveEarliestItem(){

	WOscQueueItem* result;

	int smallestIdx = 0;

	if ( m_itemsInList == 0 )
		throw new WOscException(
			WOscException::ERR_REMOVE_FROM_EMPTY_QUEUE,
			"Trying to remove items from an empty Queue.");

	/* traverse the queue and */
	for ( int i = 0; i < m_itemsInList; i++ )
		/* compare time-tags */
		if ( *m_list[smallestIdx] > *m_list[i] )
			smallestIdx = i;

	/* save pointer before removing it */
	result = m_list[smallestIdx];

	RemoveItem(smallestIdx);

	return result;
}

/** Returns the number of items in the queue.
 * 
 * 
 * \returns
 * The number of items in the queue.
 */
int WOscPriorityQueue::GetNumItems(){
	return m_itemsInList;
}

#endif	// #if WOSC_USE_PRIORITY_QUEUE == 1
