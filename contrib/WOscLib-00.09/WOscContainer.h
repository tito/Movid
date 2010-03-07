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

/** WOscContainer and WOscContainerInfo header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.6 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef __WOSCCONTAINER_H__
#define __WOSCCONTAINER_H__


#include "WOscMethod.h"
#include "WOscLib.h"
#include "WOscPatternMatch.h"
#include "WOscInfo.h"

/*  -----------------------------------------------------------------------  */
/* 	WOscContainerInfo                                                        */
/*  -----------------------------------------------------------------------  */

/** Container/node related information.
 * 
 */
class WOSC_EXPORT WOscContainerInfo: public WOscInfo{
public:
	WOscContainerInfo(const char* description);
	WOscContainerInfo();
};


/*  -----------------------------------------------------------------------  */
/* 	WOscContainer                                                            */
/*  -----------------------------------------------------------------------  */

class WOscMethod;	// Compatibility with GCC4 (credits: Cyril Comparon)
class WOscCallbackList;

/** OSC Address-space node.
 * An OSC-address space consists of a tree of containers with methods as leaves.
 *
 * \todo More elaborate documentation of WOscContainer class.
 * 
 * \remarks
 * ALL CONTAINERS EXCEPT THE ROOT CONTAINER MUST BE ALLOCATED DYNAMICALLY,
 * since the root container deletes all its children during its destruction!!
 *
 * \see
 * See the examples for address-tree generation and deletion.
 */
class WOSC_EXPORT WOscContainer: public WOscPatternMatch {
public:

	/* Root */
	WOscContainer(WOscContainerInfo* info, int defNumChildren = 32, int defNumMethods = 32);
	
	/* Children */
	WOscContainer(WOscContainerInfo* info, WOscContainer* parent, const char* name, int defNumChildren = 32, int defNumMethods = 32);
	
	void AddContainerAlias(WOscContainer* parent, const char* alias);

	WOscCallbackList* DispatchMessage(WOscMessage* msg);

	/* Address related functions */
	WOscString GetName();
	WOscString GetAddress();

	WOscString GetAddressSpace();
	WOscString GetMethodList();

	WOscContainer* FindContainer(const char* address); // NOT pattern

	WOscContainer* FindRoot();
	bool IsRoot();

	void RemoveAll();

protected:

	friend class WOscMethod;

	virtual ~WOscContainer();
	
	void AddContainer(WOscContainer* container, const char* name, bool isAlias);
	void AddMethod(class WOscMethod* method, const char* name, bool isAlias);

	WOscCallbackList* DispatchSubMessage(const char* pattern);
	
	void DoubleChildren();
	void DoubleMethods();

    WOscContainer*		m_parent;			/**< Pointer to the parent container, NULL for root nodes.*/
    
	// children
	int					m_numChildren;		/**< Number of children stored in children array.*/
	int					m_childrenCapacity; /**< Size of children-pointer-array (capacity).*/
    char**				m_childrenNames;	/**< Names of associated children.*/
	bool*				m_isContainerAlias;	/**< Indicates when when child is an alias only.*/
    WOscContainer**		m_children;			/**< Pointers to container-children attached to this container.*/
    
	// methods
	int					m_numMethods;		/**< Number of methods attached to this container.*/
	int					m_methodCapacity;	/**< Size of children-pointer-array (capacity).*/
    char**				m_methodNames;		/**< Names of attached methods.*/
	bool*				m_isMethodAlias;	/**< True when attached method is alias only.*/
    WOscMethod**		m_methods;			/**< Pointers to methods attached to this container.*/

	WOscContainerInfo*	m_info;				/**< Information about this container.*/
	
};

#endif	// #ifndef __WOSCCONTAINER_H__
