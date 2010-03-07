/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <arpa/inet.h> header file. */
#define HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `inet_ntoa' function. */
#define HAVE_INET_NTOA 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Define to 1 if little endian. */
#define IS_LITTLE_ENDIAN 1

/* Set to 1 if compiling for cygwin */
#define OS_IS_CYGWIN 1

/* Set to 1 if compiling for linux */
#define OS_IS_LINUX 0

/* Set to 1 if compiling for MacOSX */
#define OS_IS_MACOSX 0

/* Set to 1 if compiling for Win32 */
#define OS_IS_WIN32 0

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "uli.franke@weiss.ch"

/* Define to the full name of this package. */
#define PACKAGE_NAME "WOscLib"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "WOscLib 00.07.00"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "wosclib"

/* Define to the version of this package. */
#define PACKAGE_VERSION "00.07.00"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 0 to not to use the priority queue. */
#define WOSCLIB_DYN 0

/* Define to 0 to not to use time.h as system time. */
#define WOSC_HAS_STD_TIME_LIB 1

/* Define to 0 to not to use the priority queue. */
#define WOSC_USE_PRIORITY_QUEUE 0
