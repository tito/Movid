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

/** WOscMethod and WOscMethodInfo source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.5 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

#include "WOscMethod.h"
#include "WOscException.h"
#include "WOscContainer.h"

/*  -----------------------------------------------------------------------  */
/* 	WOscMethodInfo                                                           */
/*  -----------------------------------------------------------------------  */

/** Constructs a method-info from a string.
 * The string should contain some information about the method.
 * 
 * \param description
 * String with method description.
 * 
 * \throws None.
 */
WOscMethodInfo::WOscMethodInfo(const char* description)
:WOscInfo(description)
{
}

/** Constructs an empty method-info object.
 * The internal buffer is initialized to zero.
 * 
 * \throws None.
 */
WOscMethodInfo::WOscMethodInfo()
:WOscInfo()
{
}

/*  -----------------------------------------------------------------------  */
/* 	WOscMethod                                                               */
/*  -----------------------------------------------------------------------  */

/** Constructor.
 * Osc-methods can only be constructed in presence of a parent container, since
 * method-orphans are not allowed. Additionally they have to be instantiated
 * dynamically because the OSC-address-tree (formed by WOscContainer objects)
 * handles the clean-up of all tree-elements. Therefor the destructor is private.
 * 
 * \param info
 * Info about this method.
 * 
 * \param parent
 * Pointer of the container that's going to be the parent of this method.
 * 
 * \param context
 * A void pointer for later use from within the method. This can be used to pass
 * an object pointer on which the method should operate lateron (As implemented in
 * WOscReceiver).
 * 
 * \param name
 * The OSC-pattern that will be the name of this method in the OSC-address space.
 * 
 * \throws WOscException
 * When illegal parent pointers are passed (i.e. NULL).
 */
WOscMethod::WOscMethod(WOscMethodInfo* info, 
					   WOscContainer* parent, 
					   void* context, 
					   const char* name)
{
	if ( parent == NULL )
		throw new WOscException(
			WOscException::ERR_PARENT_POINTER_NULL,
			"WOscContainer: parent pointer NULL. That's illegal. Children should have parents.");

	m_parent	= parent;
	m_info		= info;
	m_context	= context;
	m_parent->AddMethod(this, name, false);
}


/** Destructor.
 * A WOscMethod can be deleted only by WOscContainers.
 * Does nothing currently.
 */
WOscMethod::~WOscMethod()
{
}

/** Adds this method as alias to the list of methods of the
 * container passed as argument.
 *
 * \param parent
 * Parent container to which this method should be added as alias.
 *
 * \param alias
 * New (aliased) name of the method.
 * 
 * \throws WOscException
 * When passing an illegal parent pointer (i.e. NULL).
 * 
 * \see
 * WOscContainer::addContainerAlias(WOscContainer* parent, char* alias)
 */
void WOscMethod::AddMethodAlias(WOscContainer* parent, const char* alias)
{
	if ( parent == NULL ) // should not happen
		throw new WOscException(
			WOscException::ERR_PARENT_POINTER_NULL,
			"WOscMethod::addMethodAlias: Invalid (NULL) parent container passed.");

	parent->AddMethod(this, alias, true);
}

// will be overridden
/** The OSC method.
 * This is the function that will be called, when a certain OSC-address in a message
 * matches the address of this method-object. The message is then passed together
 * with a network-return-address and a timeTag which contains the point in time of the 
 * invokation.
 *
 * Each message object can access the programm-ressources without global variables
 * by using the context-pointer.
 * 
 * \param message
 * Message causing this call (eventually containing arguments).
 * 
 * \param when
 * Time of invokation (should be current time, when a priority queue, system time and
 * a fast update-rate is used.
 * 
 * \param networkReturnAddress
 * The network address where the message came from.
 *
 * \throws
 * Not in this, but perhaps in inherited classes and in their function-calls.

void WOscMethod::method(WOscMessage *message, 
						WOscTimeTag& when, 
						WOscNetReturn* networkReturnAddress){

}
 */

/** Gets the name (the single pattern, \b not the OSC-address)
 * of this node.
 * If a container is named "xyz" and it's located in the
 * OSC-address-space at "/abcd/klm/xyz/lalala", only xyz is returned.
 * 
 * \returns
 * The name of this container as WOscString.
 * 
 */
WOscString WOscMethod::GetName()
{
	WOscString name;

	/* search all children of parent for not aliased name */
	for ( int i = 0; i < m_parent->m_numMethods; i++ )
		if ( (m_parent->m_methods [i] == this) && (!(m_parent->m_isMethodAlias[i]))){
			name = m_parent->m_methodNames[i];
			break;
		}

	return name;
}


/** Returns the whole OSC-address-space as seen from root.
 * The address-space contains only addresses with valid
 * leaves (OSC-methods).
 * 
 * \returns
 * All OSC-addresses which form the OSC-address-space,
 * separated by '\n'-characters.
 * 
 */
WOscString WOscMethod::GetAddressSpace()
{
	WOscContainer* root = m_parent->FindRoot();
	return root->GetMethodList();
}


/** Returns the OSC-address of this container as seen from the root.
 * If this container should be aliased, it returns the main,
 * not aliased, address.
 * 
 * \returns
 * OSC-address as WOscString.
 * 
 * \remarks
 * Remember: Container-addresses end with '/', method-addresses
 * with no slash.
 *
 */
WOscString WOscMethod::GetAddress()
{
	WOscContainer* momentaryParent = m_parent;
	WOscString address = GetName();
	WOscString tmp;

	/* traverse tree upwards until root is reached */
	while ( momentaryParent != NULL ){
		if (momentaryParent->m_parent == NULL) break;
		WOscString tmp = momentaryParent->GetName();
		tmp += '/';
		address = tmp + address;

		momentaryParent = momentaryParent->m_parent;
	}
	tmp = '/';
	
	return tmp + address;
}


/** Returns the main parent-container (not aliased ones!).
 * For use in the method-member function to get the calling-
 * container-context.
 * 
 * \returns
 * The main parent container (not aliased).
 */
WOscContainer* WOscMethod::GetParent()
{
	return m_parent;
}


/*  -----------------------------------------------------------------------  */
/* 	WOscCallbackList                                                         */
/*  -----------------------------------------------------------------------  */

/** Default Constructor.
 * Initializes an empty linked-list (root-pointer NULL).
 */
WOscCallbackList::WOscCallbackList()
{
	m_theList = NULL;
}

/** Destructor.
 * Recursivly deletes WOscCallbackListItems hosted by this list.
 */
WOscCallbackList::~WOscCallbackList()
{
	if( m_theList != NULL )
		delete m_theList;
}

/** Adds an OSC-method to the linked list.
 * Instantiates a WOscCallbackListItem internally, initializes it with the
 * method and adds it to the linked list.
 * 
 * \param method
 * Method to be added to the linked list.
 */
void WOscCallbackList::AddMethod(WOscMethod *method)
{
	if ( m_theList == NULL )
		// no items in list. create list
		m_theList = new WOscCallbackListItem(method, NULL); // head of list has zero previous pointer
	else{
		WOscCallbackListItem* lastElement = m_theList->FindLastItem();
		lastElement->m_next = new WOscCallbackListItem(method, lastElement);
	}
}

/** Appends all list-items from another WOscCallbackList to the end of this
 * list and removes them from the other.
 * 
 * \param listToInclude
 * Callback-list to be included into this one.
 */
void WOscCallbackList::IncludeList(WOscCallbackList* listToInclude)
{
	// check if elements in list which should be included
	if ( listToInclude->m_theList == NULL )
		return;

	// check if list exists already
	if ( m_theList == NULL ){
		m_theList = listToInclude->m_theList;
		listToInclude->m_theList->m_prev = NULL;
		listToInclude->m_theList = NULL; // remove from included list		
	}else{
		WOscCallbackListItem* lastItem = m_theList->FindLastItem();
		lastItem->m_next = listToInclude->m_theList;
		listToInclude->m_theList->m_prev = lastItem;
		listToInclude->m_theList = NULL; // remove from included list		
	}
}

/** Invokes all methods in this list.
 * 
 * \param message
 * The message which caused the invokation.
 * 
 * \param when
 * Point of time of the invokation.
 *
 * \param networkReturnAddress
 * Source of the message.
 * 
 * \throws
 * Not in this nor downto the WOscMessage::method(...) but inside the method
 * if the user intends to do so. 
 */
void WOscCallbackList::InvokeAll(const WOscMessage* message, 
								 const WOscTimeTag& when,
								 const WOscNetReturn* networkReturnAddress)
{
	if ( m_theList )
		m_theList->InvokeRecursively(message, when, networkReturnAddress);
}

/** Returns the number of methods in the current callbacklist.
 * This is only interesting after adding methods.
 *
 * \returns
 * Number of methods in the callback list.
 */
int WOscCallbackList::GetNumMethods() const
{
	int n = 0;
	WOscCallbackListItem* next = m_theList;

	while ( next )
	{
		n++;
		next = next->m_next;
	}

	return n;
}

/*  -----------------------------------------------------------------------  */
/* 	WOscCallbackListItem                                                     */
/*  -----------------------------------------------------------------------  */

/** Constructor.
 * Saves a pointer to an OSC-method in this object and initializes
 * the pointer to the previous element.
 * 
 * \param theMethod
 * Pointer to the method-object.
 * 
 * \param previous
 * Previous list item. NULL for first item.
 */
WOscCallbackListItem::WOscCallbackListItem(WOscMethod* theMethod, 
										   WOscCallbackListItem* previous)
{
	m_theMethod = theMethod;
	m_next = NULL;
	m_prev = previous;
}

/** Destructor.
 * Forward-deletes all list items.
 */
WOscCallbackListItem::~WOscCallbackListItem()
{
	if ( m_next )
		delete m_next;
}

/** Finds the last item in the list.
 * 
 * \returns
 * The pointer of the last list item.
 */
WOscCallbackListItem* WOscCallbackListItem::FindLastItem()
{
	if ( m_next == NULL )
		return this;
	else
		return m_next->FindLastItem();
}

/** Invokes all callback-list items, one after another.
 * 
 * \param message
 * The message responsable for the invokation
 * 
 * \param when
 * Point of time of the invokation.
 * 
 * \param networkReturnAddress
 * Source of the message.
 *
 * \throws
 * Not this but perhaps the user in its method...
 */
void WOscCallbackListItem::InvokeRecursively(const WOscMessage* message,
											 const WOscTimeTag& when,
											 const WOscNetReturn* networkReturnAddress)
{
	m_theMethod->Method(message, when, networkReturnAddress);
	if ( m_next )
		m_next->InvokeRecursively(message, when, networkReturnAddress);
}
