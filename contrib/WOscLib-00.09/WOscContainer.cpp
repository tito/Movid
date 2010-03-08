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

/** WOscContainer and WOscContainerInfo source file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-13 17:23:19 $
 * $Revision: 1.5 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

#include "WOscContainer.h"

#include <string.h>
#include "WOscException.h"

/*  -----------------------------------------------------------------------  */
/* 	WOscContainerInfo                                                        */
/*  -----------------------------------------------------------------------  */

/** Constructs a container information.
 * 
 * \param description
 * String containing string with description of the container.
 */
WOscContainerInfo::WOscContainerInfo(const char* description)
:WOscInfo(description){

}

/** Default constructor.
 * Constructs an empty description.
 */
WOscContainerInfo::WOscContainerInfo()
:WOscInfo(){

}


/*  -----------------------------------------------------------------------  */
/* 	WOscContainer                                                            */
/*  -----------------------------------------------------------------------  */


/** Constructor for a root container.
 * Root container do not have parents (what's implicated by the name).
 * The root-container is responsible for clean-up. Deletion
 * is not allowed for WOscContainer and WOscMethod objects. The 
 * address space can be removed only as whole or be manipulated
 * by dedicated functions.
 *
 * \param info
 * Container information.
 * 
 * \param defNumChildren
 * Default internal array-size for children-references. If not sufficient
 * during later use, the array-sizes double themselves automatically.
 *
 * \param defNumMethods
 * See parameter defNumChildren.
 * 
 */
WOscContainer::WOscContainer(WOscContainerInfo* info, int defNumChildren /*= 32*/, int defNumMethods /*= 32*/){

	/* a root container does not have any parents */
	m_parent = NULL;

	/* children */
	m_numChildren		= 0;
	m_childrenCapacity	= defNumChildren;
	m_childrenNames		= new char*[defNumChildren];
	m_isContainerAlias	= new bool[defNumChildren];		// indicator when alias
	m_children			= new WOscContainer*[defNumChildren];
    
	/* methods */
	m_numMethods		= 0;
	m_methodCapacity	= defNumMethods;
	m_methodNames		= new char*[defNumMethods];
	m_isMethodAlias		= new bool[defNumMethods];
	m_methods			= new WOscMethod*[defNumMethods];

	/* container info */
	m_info = new WOscContainerInfo(*info);
}

/** Constructor for child containers.
 * Child containers must supply a parent. They get added to the parent-container list of children
 * and can be parents if constructed.
 * 
 * \param info
 * Container information.
 * 
 * \param parent
 * Pointer to the parent-container. Must not be NULL.
 * 
 * \param name
 * OSC-pattern with the name of the container. This name will be later
 * a part of the OSC-address (/the/"name"/of/mama/mia/is/method ...).
 *
 * \param defNumChildren
 * (Optional) See WOscContainer::WOscContainer(WOscContainerInfo* info, int defNumChildren, int defNumMethods)
 * 
 * \param defNumMethods
 * (Optional) See WOscContainer::WOscContainer(WOscContainerInfo* info, int defNumChildren, int defNumMethods)
 * 
 * \throws WOscException
 * If parent pointer NULL. That's illegal. Children should have parents
 * 
 * \see
 * WOscContainer::WOscContainer(WOscContainerInfo* info, int defNumChildren , int defNumMethods )
 */
WOscContainer::WOscContainer(WOscContainerInfo* info, WOscContainer* parent, const char* name, int defNumChildren /*= 32*/, int defNumMethods /*= 32*/){

	if ( parent == NULL )
		throw new WOscException(
			WOscException::ERR_PARENT_POINTER_NULL,
			"WOscContainer: Parent pointer NULL. That's illegal. Children should have parents.");

	m_parent	= parent;
	m_info		= new WOscContainerInfo(*info);

	// children
	m_numChildren		= 0;
	m_childrenCapacity	= defNumChildren;
	m_childrenNames		= new char*[defNumChildren];
	m_isContainerAlias	= new bool[defNumChildren];		// indicator when alias
	m_children			= new WOscContainer*[defNumChildren];
    
	// methods
	m_numMethods		= 0;
	m_methodCapacity	= defNumMethods;
	m_methodNames		= new char*[defNumMethods];
	m_isMethodAlias		= new bool[defNumMethods];
	m_methods			= new WOscMethod*[defNumMethods];

	m_parent->AddContainer(this, name, false);

}

/** Destructor.
 * Can be acessed only by a WOscContainer object.
 * To delete a whole container-tree, call WOscContainer::removeAll() .
 * Deletes - when starting from root - all children (containers and methods) recursivly.
 *
 * \see
 * WOscContainer::removeAll()
 */
WOscContainer::~WOscContainer(){

	int i;

	// recursive deletion ....
	
	// remove all methods
	for ( i=0; i < m_numMethods; i++ ){
		if ( ! m_isMethodAlias[i] ){ // if alias, don't delete
			delete m_methods[i];
		}
		delete [] m_methodNames[i];
	}

	// remove all children
	for ( i=0; i < m_numChildren; i++ ){
		if ( ! m_isContainerAlias[i] ){ // if alias, don't delete
			delete m_children[i];
		}
		delete [] m_childrenNames[i];
	}

	// clean up
	delete [] m_childrenNames;
	delete [] m_isContainerAlias;
	delete [] m_children;
	delete [] m_methodNames;
	delete [] m_isMethodAlias;
	delete [] m_methods;

	delete m_info;
}


/** Public function for deletion of a whole container-/method-tree.
 * Searches the root and starts a recursive deletion.
 * 
 * \see
 * WOscContainer::~WOscContainer()
 */
void WOscContainer::RemoveAll(){
	delete FindRoot();
}


/** Adds an alias of this container to the parent list-of-children.
 * 
 * \param parent
 * Pointer to container, which becomes the parent of the alias.
 * 
 * \param alias
 * New name of container under which the parent saves the alias.
 * 
 * \throws WOscException
 * When this container is the root-container, the root-container can not be aliased.
 * When the parent pointer is this container itself. 
 * When invalid parent-container pointer (NULL) is passed.
 * 
 * \remarks
 * Aliases are not real children and won't be deleted during recursive deletion.
 */
void WOscContainer::AddContainerAlias(WOscContainer* parent, const char* alias){

	if ( m_parent == NULL )
		throw new WOscException(
			WOscException::ERR_ROOT_ALIAS,
			"This is the root container and the root container can not be aliased.");

	if ( parent == this )
		throw new WOscException(
			WOscException::ERR_RECURSIVE_ALIAS,
			"Aliasing itself not allowed.");
	
	if ( parent == NULL ) 
		throw new WOscException(
			WOscException::ERR_PARENT_POINTER_NULL,
			"WOscContainer::addContainerAlias: Invalid (NULL) parent container passed.");

	parent->AddContainer(this, alias, true);
}


/** Function for container-internal adding of method-children.
 * When the WOscMethod constructor is called, it internally
 * calls this function by dereferencing the parent-pointer-argument.
 * 
 * \param method
 * Pointer to method to be added to the list of methods in this container.
 * 
 * \param name
 * String containing the OSC-name of the method.
 * 
 * \param isAlias
 * If the method should be added as alias, this argument is true.
 * 
 * \see
 * WOscContainer::addContainer(WOscContainer* container, char* name, bool isAlias)
 */
void WOscContainer::AddMethod(WOscMethod* method, const char* name, bool isAlias){
	
	// manage capacity
	if ( m_numMethods == m_methodCapacity )
		DoubleMethods();

	// copy method reference to method list
	m_methods[m_numMethods] = method;
	// copy name
	m_methodNames[m_numMethods] = new char[strlen(name) + 1];
	strcpy(m_methodNames[m_numMethods], name);
	m_isMethodAlias[m_numMethods] = isAlias;

	m_numMethods++;
}

/** Adds a container to the child-container array.
 * Privately accessable and used by children
 * to add itself to the parents array of children or aliases.
 * 
 * \param container
 * Pointer of container to be added to the list.
 * 
 * \param name
 * OSC-pattern with the name of the node/container.
 * 
 * \param isAlias
 * True if added pointer is only an alias (will not be deleted when cleaning up).
 * 
 * \see
 * WOscContainer::addMethod(WOscMethod* method, char* name, bool isAlias)
 */
void WOscContainer::AddContainer(WOscContainer* container, const char* name, bool isAlias){
	// manage capacity
	if ( m_numChildren == m_childrenCapacity )
		DoubleChildren();

	// copy method reference to method list
	m_children[m_numChildren] = container;
	// copy name
	m_childrenNames[m_numChildren] = new char[strlen(name) + 1];
	strcpy(m_childrenNames[m_numChildren], name);
	m_isContainerAlias[m_numChildren] = isAlias;

	m_numChildren++;
}

/** Tries to find a certain container by its OSC-address.
 * \b Not pattern but address (/hello/world/whats/up).
 * 
 * \param address
 * String containing OSC-address to be searched for.
 * 
 * \returns
 * Pointer to container if found else NULL.
 *
 * \todo
 * Verify/test this function.
 */
WOscContainer* WOscContainer::FindContainer(const char* address)
{
	unsigned int addrlen = strlen(address);
	
	if ( addrlen == 0 )						// no address
		return NULL;
	if ( address[0] != '/' )				// invalid address
		return NULL;
	if ( addrlen == 1 && *address == '/' )	// root container "/"
		return this;
	
	// find next slash (if any)
	const char* next = address+1;
	while ( *next != 0 && *next != '/' )
		next++;
	
	if ( *next == 0 ) {
		// leaf
		for (int i = 0; i < m_numChildren; i++)
			if ( strcmp(m_childrenNames[i], address+1) == 0 )
				return m_children[i];
	} else {
		// not a leaf. extract node name
		unsigned int thisNodeStrLen = next-address;
		char* thisNodeStr = new char[thisNodeStrLen+1];
		strncpy(thisNodeStr, address, thisNodeStrLen);
		thisNodeStr[thisNodeStrLen] = 0;
		for (int i = 0; i < m_numChildren; i++) {
			if ( strcmp(m_childrenNames[i], thisNodeStr+1) == 0 ) {
				WOscContainer* ctnr = m_children[i]->FindContainer(next);
				if ( ctnr ) {
					delete [] thisNodeStr;
					return ctnr;
				}
			}
		}
		delete [] thisNodeStr;
	}
		
	return NULL;
}


/** If during add children/children-alias the container-internal container-children-arrays
 * get too small, their capacity is doubled by calling this function.
 */
void WOscContainer::DoubleChildren(){
	// allocate larger list
	int newCapacity = 2 * m_numChildren;
	WOscContainer** newChildren = new WOscContainer*[newCapacity];

	// copy old elements
	int i = 0;
	for ( ; i < m_numChildren; i++ )
		newChildren[i] = m_children[i];

	// init new elements
	for ( ; i < newCapacity; i++ )
		newChildren[i] = NULL;

	// activate new list and delete old
	WOscContainer** moribundus = m_children;
	m_children = newChildren;
	m_numChildren = newCapacity;
	delete [] moribundus;
}

/** If during add method/method-alias the container-internal method-children-arrays
 * get too small, their capacity is doubled by calling this function.
 */
void WOscContainer::DoubleMethods(){
	// allocate larger list
	int newCapacity = 2 * m_numMethods;
	WOscMethod** newMethods = new WOscMethod*[newCapacity];

	// copy old elements
	int i = 0;
	for ( ; i < m_numMethods; i++ )
		newMethods[i] = m_methods[i];

	// init new elements
	for ( ; i < newCapacity; i++ )
		newMethods[i] = NULL;

	// activate new list and delete old
	WOscMethod** moribundus = m_methods;
	m_methods = newMethods;
	m_numMethods = newCapacity;
	delete [] moribundus;
}

/** Creates a list of methods which match the address of the message passed
 * as argument. Those lists reflect the effect of an OSC-message.
 * The list can then be used to invoke all matching methods.
 * 
 * \param msg
 * Message to be scanned for.
 * 
 * \returns
 * A list containing all matching functions. Its the task of the list
 * if none are found and an invocation is issued.
 * 
 * \throws WOscException
 * When dispatching is not executed from a root container.
 * (can be removed by first searching for root. discussion?).
 *
 * \remarks
 * Code stolen from Matt Wright's OSC-Kit and adapted to this architecture (\b Thank You!).
 *
 * \see
 * WOscContainer::dispatchSubMessage(char* pattern)
 */
WOscCallbackList* WOscContainer::DispatchMessage(WOscMessage* msg){
	
	WOscString addrPattern = msg->GetOscAddress();
	const char* const pattern = addrPattern.GetBuffer();

	if ( m_parent != NULL )
		throw new WOscException(
			WOscException::ERR_NOT_DISPATCHING_FROM_ROOT,
			"Can dispatch messages only when called from root container!");

	if (pattern[0] != '/')
		throw new WOscException(
			WOscException::ERR_INVALID_ADDRESS_NO_SLASH,
			"Invalid address. Does not begin with /");

	// copy address into temporary buffer where the find-algorithm can modify it
	char* tmpPattern = new char[strlen(pattern)+1];
	strcpy(tmpPattern,pattern);

	WOscCallbackList* result = DispatchSubMessage(tmpPattern+1);
	delete [] tmpPattern;
	return result;
}


/** Creates a list of methods which match the address passed as argument.
 * 
 * 
 * \param pattern
 * OSC-sub-address to be scanned for. If the first pattern matches a child the remaining
 * address will be passed tho that child (recursion).
 *
 * \returns
 * A list of methods matching the pattern.
 * 
 * \remarks
 * Code stolen from Matt Wright's OSC-Kit and adapted to this architecture (\b Thank You!).
 * 
 * \see
 * WOscContainer::dispatchMessage(WOscMessage* msg)
 */
WOscCallbackList* WOscContainer::DispatchSubMessage(const char* pattern){

	WOscCallbackList* resultingMethodList = new WOscCallbackList();
	const char *nextSlash, *restOfPattern;
	int i;

	nextSlash = NextSlashOrNull(pattern);

	if (*nextSlash == '\0') {
		// leaves
		/* Base case: the pattern names methods of this container. */
		for (i = 0; i < m_numMethods; i++)
			if (PatternMatch(pattern, m_methodNames[i]))
				// method found -> add to method list
				resultingMethodList->AddMethod(m_methods[i]);
	} else {
		/* Recursive case: in the middle of an address, so the job at this 
			step is to look for containers that match.  We temporarily turn
			the next slash into a null so pattern will be a null-terminated
			string of the stuff between the slashes. */
		*((char*)nextSlash) = '\0';	// this is not clean and should be fixed somehow (below as well)
		restOfPattern = nextSlash + 1;

		for (i = 0; i < m_numChildren; ++i)
			if (PatternMatch(pattern, m_childrenNames[i])){
				WOscCallbackList* list2Include = m_children[i]->DispatchSubMessage(restOfPattern);
				resultingMethodList->IncludeList( list2Include );
				delete list2Include;
			}
		*((char*)nextSlash) = '/';
	}
	return resultingMethodList;
}

/** Returns the not aliased name (OSC-pattern, \b not address) of this container.
 * 
 * \returns
 * WOscString containing the name-pattern of this container.
 */
WOscString WOscContainer::GetName(){
	WOscString name("/");

	if ( m_parent == NULL ){
		return name;
	}
	
	/* search all children of parent for not aliased name */
	for ( int i = 0; i < m_parent->m_numChildren; i++ )
		if ( (m_parent->m_children[i] == this) && (!(m_parent->m_isContainerAlias[i]))){
			name = m_parent->m_childrenNames[i];
			break;
		}

	return name;
}

/** Traverses the container-tree upwards until root is reached and
 * assembles its unaliased address.
 * 
 * \returns
 * OSC-address of this container.
 *
 * \remarks
 * OSC-addresses of containers end with a slash ('\\'),
 * OSC-addresses of methods end with nothing.
 */
WOscString WOscContainer::GetAddress(){

	if ( m_parent == NULL)
		return WOscString("/");

	WOscContainer* momentaryParent = m_parent;
	WOscString address = GetName();
	address += '/';

	/* traverse tree upwards until root is reached */
	while ( momentaryParent != NULL ){

		address = momentaryParent->GetName() + address;

		momentaryParent = momentaryParent->m_parent;
	}
	
	return address;
}

/** Returns a line-break('\\n')-separated list of all OSC-addresses of 
 * methods as seen from root.
 * 
 * \returns
 * WOscString containing the list.
 *
 * \remarks
 * OSC-addresses of containers end with a slash ('\\'),
 * OSC-addresses of methods end with nothing.
 */
WOscString WOscContainer::GetAddressSpace(){
	WOscContainer* root = FindRoot();
	return root->GetMethodList();
}


/** Scans for all local methods and aliased methods.
 * 
 * \returns
 * A list as described in WOscContainer::getAddressSpace() containing
 * the addresses of all method-leaves reachable from this node/container.
 */
WOscString WOscContainer::GetMethodList(){
	WOscString methodList;

	// scan for local methods
	for ( int m = 0; m < m_numMethods; m++ ){
		if ( m_isMethodAlias[m] ){
			methodList += GetAddress();
			methodList += m_methodNames[m];
		}else{
			methodList += m_methods[m]->GetAddress();
		}
		methodList += '\n';
	}

	// scan children containers
	for ( int c = 0; c < m_numChildren; c++ ){
		if ( m_isContainerAlias[c] ){
			/** \todo Does not handle aliased containers !!!! */
		}else{
			methodList += m_children[c]->GetMethodList();
		}
	}
	
	return methodList;
}

/** Traverses the container-tree upwards until root is reached.
 * 
 * \returns
 * The pointer of the root-container.
 */
WOscContainer* WOscContainer::FindRoot(){

	WOscContainer* momentaryParent = m_parent;
	WOscContainer* lastParent = this;

	/* traverse tree upwards until root is reached */
	while ( momentaryParent != NULL ){
		lastParent = momentaryParent;
		momentaryParent = momentaryParent->m_parent;
	}
	return lastParent;
}


/** Checks if the current container is the root-container.
 * 
 * \returns
 * True if it is, false else.
 */
bool WOscContainer::IsRoot(){
	return m_parent == NULL ? true : false;
}

