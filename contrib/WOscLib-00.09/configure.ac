#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([WOscLib], [00.07.00], [uli.franke@weiss.ch])
AC_CONFIG_AUX_DIR(cfg)
#AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([WOscBlob.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_CANONICAL_TARGET([])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

AC_C_BIGENDIAN([AC_DEFINE([IS_LITTLE_ENDIAN], [0],[Define to 0 if big endian.])], [AC_DEFINE([IS_LITTLE_ENDIAN], [1],[Define to 1 if little endian.])], [AC_MSG_ERROR([Unknown endianness])], [AC_MSG_ERROR([Universal build is not supported])])

# Checks for library functions.
AC_CHECK_FUNCS([inet_ntoa socket strchr])

#==============================================================================
# lib features
AC_ARG_WITH([time_h],
	[AS_HELP_STRING([--with-time_h], [Use time_h as system time (default is yes)])],
	[use_time_h=$withval],
	[use_time_h=yes])
AS_IF([test "$use_time_h" = no],
	[AC_DEFINE([WOSC_HAS_STD_TIME_LIB], [0], [Define to 1 to use time.h as system time.])],
	[AC_DEFINE([WOSC_HAS_STD_TIME_LIB], [1], [Define to 0 to not to use time.h as system time.])])

AC_ARG_WITH([priority_queue],
	[AS_HELP_STRING([--with-priority_queue], [Enable use of the priority queue (default is no)])],
	[use_priority_queue=$withval],
	[use_priority_queue=no])
AS_IF([test "$use_priority_queue" = no],
	[AC_DEFINE([WOSC_USE_PRIORITY_QUEUE], [0], [Define to 1 to use the priority queue.])],
	[AC_DEFINE([WOSC_USE_PRIORITY_QUEUE], [1], [Define to 0 to not to use the priority queue.])])

AC_ARG_WITH([dynamic_link],
	[AS_HELP_STRING([--with-dynamic_link], [Enable build of dynamic library (default is no)])],
	[use_dynamic_link=$withval],
	[use_dynamic_link=no])
AS_IF([test "$use_dynamic_link" = no],
	[AC_DEFINE([WOSCLIB_DYN], [0], [Define to 1 to use the priority queue.])],
	[AC_DEFINE([WOSCLIB_DYN], [1], [Define to 0 to not to use the priority queue.])])

#==============================================================================
# Target OS specific stuff.

OS_SPECIFIC_CFLAGS=""
OS_SPECIFIC_LIBSUFFIX=""
OS_SPECIFIC_LIBLINKS=""
OS_SPECIFIC_EXAMPLELINKS=""
OS_SPECIFIC_EXAMPLESUFFIX=""
os_is_win32=0
os_is_macosx=0
os_is_linux=0
os_is_cygwin=0

case "$host_os" in
	linux*)
		os_is_linux=1
		OS_SPECIFIC_CFLAGS=""
		OS_SPECIFIC_LIBLINKS=""
		if test "$use_dynamic_link" = "no" ; then
			OS_SPECIFIC_LIBSUFFIX=".a"
			OS_SPECIFIC_EXAMPLELINKS="-lwosclib"
		else
			OS_SPECIFIC_LIBSUFFIX=".so"
			OS_SPECIFIC_LIBLINKS="-shared"
			OS_SPECIFIC_EXAMPLELINKS="-lwosclib"
		fi
		;;
	darwin* | rhapsody*)
		os_is_macosx=1
		OS_SPECIFIC_CFLAGS=""
		OS_SPECIFIC_LIBLINKS=""
		if test "$use_dynamic_link" = "no" ; then
			OS_SPECIFIC_LIBSUFFIX=".a"
			OS_SPECIFIC_EXAMPLELINKS="-lwosclib -framework CoreServices"
		else
			OS_SPECIFIC_LIBSUFFIX=".dylib"
			OS_SPECIFIC_LIBLINKS="-dynamiclib"
			OS_SPECIFIC_EXAMPLELINKS="-lwosclib -framework CoreServices"
		fi
		;;
	mingw*)
		os_is_win32=1
		OS_SPECIFIC_CFLAGS=""
		if test "$use_dynamic_link" = "no" ; then
			OS_SPECIFIC_LIBSUFFIX=".a"
			OS_SPECIFIC_LIBLINKS=""
			OS_SPECIFIC_EXAMPLELINKS="-lws2_32 -lwosclib"
		else
			OS_SPECIFIC_LIBSUFFIX=".dll"
			OS_SPECIFIC_LIBLINKS="-shared --out-implib"
			OS_SPECIFIC_EXAMPLELINKS="-lws2_32 -lwosclib"
		fi
		OS_SPECIFIC_EXAMPLESUFFIX=".exe"
		;;
	cygwin*)
		os_is_cygwin=1
		OS_SPECIFIC_CFLAGS=""
		if test "$use_dynamic_link" = "no" ; then
			OS_SPECIFIC_LIBSUFFIX=".a"
			OS_SPECIFIC_LIBLINKS=""
			OS_SPECIFIC_EXAMPLELINKS="-lwosclib"
		else
			OS_SPECIFIC_LIBSUFFIX=".dll"
			OS_SPECIFIC_LIBLINKS="-shared --out-implib"
			OS_SPECIFIC_EXAMPLELINKS="-lwosclib"
		fi
		OS_SPECIFIC_EXAMPLESUFFIX=".exe"
		;;
	esac

AC_DEFINE_UNQUOTED(OS_IS_WIN32, ${os_is_win32}, [Set to 1 if compiling for Win32])
AC_DEFINE_UNQUOTED(OS_IS_MACOSX, ${os_is_macosx}, [Set to 1 if compiling for MacOSX])
AC_DEFINE_UNQUOTED(OS_IS_LINUX, ${os_is_linux}, [Set to 1 if compiling for linux])
AC_DEFINE_UNQUOTED(OS_IS_CYGWIN, ${os_is_cygwin}, [Set to 1 if compiling for cygwin])

#==============================================================================
# makefile processing

AC_SUBST(OS_SPECIFIC_CFLAGS)
AC_SUBST(OS_SPECIFIC_LIBSUFFIX)
AC_SUBST(OS_SPECIFIC_LIBLINKS)
AC_SUBST(OS_SPECIFIC_EXAMPLELINKS)
AC_SUBST(OS_SPECIFIC_EXAMPLESUFFIX)


AC_OUTPUT

#==============================================================================
# print summary

echo " "
echo "-=-=-=-=-=-=-=-=-=-= Configuration Complete =-=-=-=-=-=-=-=-=-=-"
echo " "
#    echo "    Version : ............................. ${VERSION}"
if test "$use_dynamic_link" = "no" ; then
	echo "    Link : ................................ static"
else
	echo "    Link : ................................ dynamic"
fi
if test "$use_priority_queue" = "no" ; then
	echo "    Priority queue : ...................... disabled"
else
	echo "    Priority queue: ....................... enabled"
fi
if test "$use_time_h" = "no" ; then
	echo "    time.h as system time : ............... no"
else
	echo "    time.h as system time: ................ yes"
fi
