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
 * $Revision: 1.5 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCMETHOD_H__
#define __WOSCMETHOD_H__

#include "WOscNetReturn.h"
#include "WOscLib.h"
#include "WOscMessage.h"
#include "WOscTimeTag.h"
#include "WOscContainer.h"
#include "WOscInfo.h"

/*  -----------------------------------------------------------------------  */
/* 	WOscMethodInfo                                                           */
/*  -----------------------------------------------------------------------  */

/** Information about an OSC method.
 * When queried for information about a method, this class defines it.
 *
 */
class WOSC_EXPORT WOscMethodInfo: public WOscInfo{
public:
	WOscMethodInfo(const char* description);
	WOscMethodInfo();
};

/*  -----------------------------------------------------------------------  */
/* 	WOscMethod                                                               */
/*  -----------------------------------------------------------------------  */

class WOscContainer;

/** OSC callback methods are objects that are mapped to a specific OSC-address.
 * Very similar to OSC-containers they are children of any container.
 *
 * If during dispatching and subsequent invoking an OSC-method gets called, the
 * message, which produced the invokation, and a network-return-address is passed.
 *
 * Methods can be seen as remote functions. Multiple computers communicate remotely 
 * by calling each others functions and passing arguments.
 *
 * Inherit your own methods from this class or - when using WOscReceiver - from WOscReceiverMethod.
 * 
 * \see
 * WOscCallbackList | WOscCallbackListItem | WOscReceiverMethod
 */
class WOSC_EXPORT WOscMethod{
public:

	WOscMethod(WOscMethodInfo* info, WOscContainer* parent, void* context, const char* name);

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
	*/
	virtual void Method(const WOscMessage* message, const WOscTimeTag& when, const WOscNetReturn* networkReturnAddress) = 0;

	WOscContainer* GetParent();

	void AddMethodAlias(WOscContainer* parent, const char* alias);

	/* Address related functions */
	WOscString GetName();
	WOscString GetAddress();

	WOscString GetAddressSpace();


protected:
	/** WOscContainer controls the deletion of registered methods.
	 */
	friend class WOscContainer;
	virtual ~WOscMethod();

	void*			m_context;	/**< Freely usable context pointer.*/
	WOscMethodInfo*	m_info;		/**< Info object.*/

private:
	WOscMethod();				/**< Disabled default constructor.*/
	WOscMethod(WOscMethod &rhs);/**< Disabled copy constructor.*/

	WOscContainer*	m_parent;	/**< Parent container.*/

};

/** Forms a linked list which is privately constructed 
 * and maintained by WOscCallbackList.
 * 
 * A list item contains a pointer to a method which matches
 * a given OSC-address like all methods in the same list.
 *
 * \see
 * WOscCallbackList | WOscMethod
 */
class WOSC_EXPORT WOscCallbackListItem{
private:
	
	friend class WOscCallbackList;
	WOscCallbackListItem(WOscMethod* theMethod, WOscCallbackListItem* previous);
	virtual ~WOscCallbackListItem();
	WOscCallbackListItem* FindLastItem();
	void InvokeRecursively(const WOscMessage* message, const WOscTimeTag& when, const WOscNetReturn* networkReturnAddress);

	WOscCallbackListItem*	m_prev;			/**< Pointer to previous list element.*/
	WOscCallbackListItem*	m_next;			/**< Pointer to next list element.*/

	WOscMethod*				m_theMethod;	/**< Pointer to the method this list item is representing.*/
};

/** A linked list of WOscMethod pointers. 
 * When dispatching messages from a OSC-address-space all
 * matching methods are returned in a WOscCallbackList object.
 * Linked lists make it simple to add and invoke a dynamic
 * amount of methods (if for instance regular expressions are used in the OSC-addresses).
 * 
 * If methods get added during dispatching, they can be later invoked by calling
 * void invokeAll(const WOscMessage* message, const WOscTimeTag& when, const WOscNetReturn* networkReturnAddress) 
 * even if no methods were found.
 *
\verbatim
+-----------------+
|  callbacklist   |
+=================+
| pointer to list |
+----------------\
                  \  (NULL if empty)
                   v
                    +----------------------+                     +----------------------+
                    | callbacklistitem     |           +-------->| callbacklistitem     |
                    +======================+           |         +======================+
                    | pointer to method    |           |         | pointer to method    |
                    +----------------------+           |         +----------------------+
                    | pointer to next item ------------+         | pointer to next item --- ...
                    +----------------------+    (NULL if empty)  +----------------------+   
\endverbatim
 *
 * \see
 * WOscCallbackListItem | WOscMethod
 */
class WOSC_EXPORT WOscCallbackList{
public:
	
	WOscCallbackList();
	virtual ~WOscCallbackList();
	void AddMethod(WOscMethod *method);
	void IncludeList(WOscCallbackList* listToInclude);
	void InvokeAll(const WOscMessage* message, const WOscTimeTag& when, const WOscNetReturn* networkReturnAddress);

	int GetNumMethods() const;

private:
	WOscCallbackListItem* m_theList; /**< Pointer to the linked list of WOscCallbackListItem s.*/

};

#endif	// #ifndef __WOSCMETHOD_H__

