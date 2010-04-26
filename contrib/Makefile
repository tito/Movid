LIBEVENT_PATH = libevent-1.4.13-stable
LIBEVENT_LIB = ${LIBEVENT_PATH}/.libs/libevent.a

LIBFIDTRACK_PATH = libfidtrack
LIBFIDTRACK_LIB = ${LIBFIDTRACK_PATH}/libfidtrack.a

WOSCLIB_PATH = WOscLib-00.09
WOSCLIB_LIB = ${WOSCLIB_PATH}/libwosclib.a

PTYPES_PATH = ptypes-2.1.1
PTYPES_LIB = ${PTYPES_PATH}/lib/libptypes.a

all: ptypes libevent fidtrack wosclib

clean:
	-${MAKE} -C ${LIBEVENT_PATH} distclean
	-${MAKE} -C ${WOSCLIB_PATH} clean
	-${MAKE} -C ${PTYPES_PATH} clean
	-${MAKE} -C ${LIBFIDTRACK_PATH} clean

#
# LIBEVENT
#

libevent: ${LIBEVENT_LIB}

${LIBEVENT_LIB}:
	cd ${LIBEVENT_PATH} && sh ./configure --disable-shared && ${MAKE}

#
# FIDTRACK
#

fidtrack: ${LIBFIDTRACK_LIB}

${LIBFIDTRACK_LIB}:
	${MAKE} -C ${LIBFIDTRACK_PATH}

#
# WOSCLIB
#

wosclib: ${WOSCLIB_LIB}

${WOSCLIB_LIB}:
	cd ${WOSCLIB_PATH} && sh ./configure && ${MAKE} libwosclib.a

#
# PTYPES
#

ptypes: ${PTYPES_LIB}

${PTYPES_LIB}:
	cd ${PTYPES_PATH} && make

