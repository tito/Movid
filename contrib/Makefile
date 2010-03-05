JPEG_ARCHIVE = jpegsrc.v8.tar.gz
JPEG_PATH = jpeg-8
JPEG_LIB = ${JPEG_PATH}/.libs/libjpeg.a

LIBEVENT_ARCHIVE = libevent-1.4.13-stable.tar.gz
LIBEVENT_PATH = libevent-1.4.13-stable
LIBEVENT_LIB = ${LIBEVENT_PATH}/.libs/libevent.a

LIBFIDTRACK_PATH = libfidtrack
LIBFIDTRACK_LIB = ${LIBFIDTRACK_PATH}/libfidtrack.a

WOSCLIB_ARCHIVE = WOscLib-00.09.zip
WOSCLIB_PATH = WOscLib-00.09
WOSCLIB_LIB = ${WOSCLIB_PATH}/libwosclib.a

PTYPES_ARCHIVE = ptypes-2.1.1.tar.gz
PTYPES_PATH = ptypes-2.1.1
PTYPES_LIB = ${PTYPES_PATH}/lib/libptypes.a

all: ptypes libevent jpeglib fidtrack wosclib

clean:
	-rm -rf ${LIBEVENT_PATH}
	-rm -rf ${JPEG_PATH}
	-rm -rf ${WOSCLIB_PATH}
	-rm -rf ${PTYPES_PATH}
	${MAKE} -C ${LIBFIDTRACK_PATH} clean

#
# LIBEVENT
#

libevent: ${LIBEVENT_LIB}

${LIBEVENT_LIB}:
	tar xzf ${LIBEVENT_ARCHIVE}
	cd ${LIBEVENT_PATH} && ./configure --disable-shared && ${MAKE}

#
# JPEGLIB
#

jpeglib: ${JPEG_LIB}

${JPEG_LIB}:
	tar xzf ${JPEG_ARCHIVE}
	cd ${JPEG_PATH} && ./configure && ${MAKE}

#
# FIDTRACK
#

fidtrack:
	${MAKE} -C ${LIBFIDTRACK_PATH}

#
# WOSCLIB
#

wosclib: ${WOSCLIB_LIB}

${WOSCLIB_LIB}:
	unzip ${WOSCLIB_ARCHIVE}
	cd ${WOSCLIB_PATH} && ./configure && ${MAKE} libwosclib.a

#
# PTYPES
#

ptypes: ${PTYPES_LIB}

${PTYPES_LIB}:
	tar xzf ${PTYPES_ARCHIVE}
	cd ${PTYPES_PATH} && make

