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

/** WOscLib library main header file.
 * Main preprocessor defines, globals (if any) and doxygen main-page.
 *
 * \file
 *
 * $Author: cls-nebadje $ ( \ref _UcfWOscLib )
 * $Date: 2009-01-14 17:16:49 $
 * $Revision: 1.5 $
 *
 * Copyright (c) Weiss Engineering Ltd
 *
 */

/**
 * \mainpage
 * 
 **
 * \section _WOscLibIntro Introduction
 *
 * This is the documentation of the Weiss OSC Library (WOscLib).
 * WOscLib is a complete (server and client), easy to use OSC 
 * (OpenSound-Control) library, featuring Object Oriented (C++) design 
 * (modularity), platform independence, type-safety (especially in OSC-methods), 
 * exception-handling and good documentation (doxygen).
 *
 * This library is the result of the development of an OSC system at 
 * Weiss Engineering LTD. in Switzerland. DWE likes to share its 
 * knowledge and team up with other developers to maintain an enhanceable 
 * library, accessible through a global cvs or similar version control system.
 *
 * \subsection _WOscLibImplementations Implementation 
 *
 * This work is a re-implementation and (hopefully) modernization of the 
 * the classic OSC-Kit, which was originally provided by Matt-Wright and 
 * was written in pure C (see http://www.cnmat.berkeley.edu/OpenSoundControl).
 *
 * Matt's kit uses lots of global variables, a fast but user-unfriendly 
 * memory-allocation (and de-allocation)-scheme, makes use of non type-save 
 * osc-callback-functions, has no real exception handling and its modularity 
 * is heavily restricted due to the C-design. There is also no documentation 
 * based on a modern auto-doc-system (e.g. doxygen) what makes it harder for 
 * newbies to get an OSC-system running in 10 minutes since they have to read 
 * all (or at least some) source-files first.
 *
 * Summary of the reasons of the re-implementation are:
 *		- Higher level programming language (C++) and therefor a higher productivity.
 *		- Enhanced modularity.
 *		- Enhanced code-reusage.
 *		- Elimination of global variables and functions to facilitate
 *			usage of multiple OSC-servers in the same process.
 *		- Good exception handling.
 *		- Type-save interfaces for OSC arguments in OSC-methods.
 *		- Better (and dynamic) management of OSC-methods.
 *		- Good documentation.
 *		- Less complex OSC-system implementation.
 *
 **
 * \section _WOscLibPlatforms Platform Dependency Issues
 * The Libraries only platformdependencies are located in WOscUtil and 
 * WOscTimeTag resp.. See documentation of those classes for details.
 * 
 * \subsection Platforms
 * Currently supported platforms are:
 *		- Most platforms supported by the GNU Compiler Collection (using g++)
 *		- MSVC (Microsoft Visual C++ Compiler)
 *
 **
 * \section _WOscLibLibraryDependencies Library Dependencies
 *		- <string.h>: WOscBlob.cpp, WOscBundle.cpp, WOscContainer.cpp, WOscException.cpp, WOscInfo.cpp, WOscMessage.cpp, WOscString.cpp, WOscUtil.cpp (memcpy, strcmp, strlen, strcpy) 
 *		- <time.h>: WOscTimeTag.cpp can depend on  (see define WOSC_HAS_STD_TIME_LIB in this file)
 *
 * \section _WOscLibNotes Acknowledgements
 * The authors like to thank Matt Wright for his OSC-Kit, Daniel Weiss for his consent
 * to make this library open-source and all contributors.
 *
 *
 * \section _WOscCopyRight Copyright
 *
 * \verbatim

 WOscLib, an object oriented OSC library.
 Copyright (C) 2005 Uli Clemens Franke, Weiss Engineering LTD, Switzerland.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 \endverbatim
 *
 * See \ref WOscLibLgplPage "LGPL" for details.
 *
 * Parts of the library are based on work of Matt Wright. His code is included
 * under the following copyright:
 *
 * \verbatim

 Copyright © 1998. The Regents of the University of California (Regents). 
 All Rights Reserved.

 Written by Matt Wright, The Center for New Music and Audio Technologies,
 University of California, Berkeley.

 Permission to use, copy, modify, distribute, and distribute modified versions
 of this software and its documentation without fee and without a signed
 licensing agreement, is hereby granted, provided that the above copyright
 notice, this paragraph and the following two paragraphs appear in all copies,
 modifications, and distributions.

 IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

 \endverbatim
 *
 *
 */

/**
 * \page osc_authors_page Authors
 *
 * Authors related to this project:
 *
 * \section _UcfWOscLib ucf
 * Uli Clemens Franke, Weiss Engineering LTD.
 *
 **
 * \page WOscLibOsc_spec_page OSC Specification
 * \htmlinclude OSC-spec.html
 * \page osc_spec_examples_page OSC Specification Examples
 * \htmlinclude OSC-spec-examples.html
 *
 ** 
 * \page WOscLibLgplPage GNU Lesser General Public License
 * \htmlinclude lgpl.html
 */

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OSCLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OSCLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __WOSCLIB_H__
#define __WOSCLIB_H__

#include "config.h"

/*- switch library type -----------------------------------------------------*/

/** Library type settings (DLL, static, windows, etc). Set the corresponding
 * proprocessor defines in the makefile.
 */
#if WOSCLIB_DYN == 1 && OS_IS_WIN32 == 1
	/** when compiling library as DDL */
#	define WOSC_EXPORT __declspec(dllexport)
#elif defined(USE_WOSCLIB_DLL) && OS_IS_WIN32 == 1
	/** when using library as DLL */
#	define WOSC_EXPORT __declspec(dllimport)
#else
	/** static library and other cases */
#	define WOSC_EXPORT
#endif

/*- misc. configuration -----------------------------------------------------*/

/** System (compiler) dependent 64 bit data type (only used in time tag).
 */
#ifdef _MSC_VER
	/** Microsoft compiler specific 64-bit data-type. */
#	define WOSCLIB_UINT64	unsigned __int64
#	define WOSCLIB_UINT32	unsigned __int32	
#elif defined( __GNUG__ )
	/** GCC */
#	define WOSCLIB_UINT64	unsigned long long int
#	define WOSCLIB_UINT32	unsigned long int
#endif

/** Overloading new and delete globally for custom memory pools.
 * Do not define it here (in the main header file) but in a
 * separate include file for not affecting the code
 * which uses this library.
 *
 * \verbatim 
#include <stdlib.h>		// memory
inline void * operator new(size_t size)
{
        return malloc(size);
};
inline void operator delete(void * mem)
{
        free(mem);
};
\endverbatim
 *
 */

#endif // #ifndef __WOSCLIB_H__

