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

/**  WOscBundle header file.
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2006-05-16 21:21:41 $
 * $Revision: 1.2 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */
#ifndef	__WOSCBUNDLE_H__
#define __WOSCBUNDLE_H__

#include "WOscPriorityQueue.h"
#include "WOscLib.h"
#include "WOscMessage.h"
#include "WOscPacket.h"

/** An OSC bundle bundles messages and bundles together which
 * have to be invoked at the same time.
 * 
 * OSC-bundles are priority-queue elements just in case a priority 
 * queue is used. The network return address can be added to
 * beeing able to pass it to the later invoked method, when it was 
 * queued in a priority queue.
 *
 * \remarks
 * A bundle element (Message or Bundle) is saved internally
 * as a reference. If the element is not removed by GetBundle
 * or GetMessage, the destructor cleans them up.
 * This speeds up the passing around of elements because
 * only the references (pointers) have to be managed/copied.
 * 
 * \see
 * None.
 */
class WOSC_EXPORT WOscBundle: public WOscQueueItem, public WOscPacket
{

public:
	/* construct from network stream (or other) */
	WOscBundle(const char* rawData, int dataLen, WOscNetReturn* ra);

	/* construct for transmission */
	WOscBundle();
	WOscBundle(WOscTimeTag timeTag);

	virtual ~WOscBundle();

	void Add(WOscBundle* bundle);
	void Add(WOscMessage* message);

	int GetNumMessages();
	int GetNumBundles();

	WOscMessage* GetMessage(int idx);
	WOscBundle* GetBundle(int idx);

	virtual void GetBuffer(char* buffer, int bufferLen);
	virtual const char* GetBuffer();
	virtual int GetBufferLen();
	
	/** Bundle related constants.
	 */
	enum Constants{
		END_OF_BUFFER = -1,		/**< Return value for CheckSize(int relReadPos, int dataLen, const char* rawData) .*/
		BUNDLE_HEADER_SIZE = 8,	/**< Size of the "#bundle\0" header at the beginnning of every bundle.*/
		SIZE_SIZE = 4,			/**< Size of bundle the bundle "size"-field which comes before every bundle element.*/
	};

private:
	void RemoveBundle(int idx);
	void RemoveMessage(int idx);

	void InitEmpty();
	void GenerateBufferFromElements();
	int CheckSize(int relReadPos, int dataLen, const char* rawData);

	WOscMessage**	m_messages;		/**<Array of message elements in this bundle.*/
	int				m_numMessages;	/**<Number of message elements in this bundle. */

	WOscBundle**	m_bundles;		/**<Array of bundle elements in this bundle.*/
	int				m_numBundles;	/**<Number of bundle elements in this bundle.*/

	char*			m_buffer;		/**<The buffer which represents the binary version of this bundle.*/
	int				m_bufferSize;	/**<Size of the buffer.*/


	WOscBundle(WOscBundle& bundle); /**< Disabled.*/

/* obsolete
	void copy(WOscBundle* bundle); // disabled because copyconstructors are disabled
	WOscBundle(WOscTimeTag timeTag, WOscMessage* message);
	WOscBundle(WOscTimeTag timeTag, WOscBundle* bundle);
	*/

};

#endif	// #ifndef	__WOSCBUNDLE_H__
