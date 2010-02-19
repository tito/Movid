JPEG_ARCHIVE = jpegsrc.v8.tar.gz
JPEG_PATH = jpeg-8
JPEG_LIB = ${JPEG_PATH}/.libs/libjpeg.a

LIBEVENT_ARCHIVE = libevent-1.4.13-stable.tar.gz
LIBEVENT_PATH = libevent-1.4.13-stable
LIBEVENT_LIB = ${LIBEVENT_PATH}/.libs/libevent.a

all: libevent jpeglib

clean:
	-rm -rf ${LIBEVENT_PATH}
	-rm -rf ${JPEG_PATH}

libevent: ${LIBEVENT_LIB}

${LIBEVENT_LIB}:
	tar xzf ${LIBEVENT_ARCHIVE}
	cd ${LIBEVENT_PATH} && ./configure --disable-shared && ${MAKE}

jpeglib: ${JPEG_LIB}

${JPEG_LIB}:
	tar xzf ${JPEG_ARCHIVE}
	cd ${JPEG_PATH} && ./configure && ${MAKE}
