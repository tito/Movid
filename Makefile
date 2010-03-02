# contrib path
CONTRIB_PATH = contrib
LIBEVENT_PATH = ${CONTRIB_PATH}/libevent-1.4.13-stable
LIBCJSON_PATH = ${CONTRIB_PATH}/cJSON
LIBJPEG_PATH = ${CONTRIB_PATH}/jpeg-8
LIBFIDTRACK_PATH = ${CONTRIB_PATH}/libfidtrack
WOSCLIB_PATH = ${CONTRIB_PATH}/WOscLib-00.09

#stuff we need to compile
TRACKER_BIN = tracker
BLOB_BIN = blobtrack
DESCRIBE_BIN = describe
DAEMON_BIN = daemon

LIBEVENT_LIBS = ${LIBEVENT_PATH}/.libs/libevent.a
LIBEVENT_CFLAGS = -I${LIBEVENT_PATH}
LIBJPEG_LIBS = ${LIBJPEG_PATH}/.libs/libjpeg.a
LIBJPEG_CFLAGS = -I${LIBJPEG_PATH}
LIBCJSON_CFLAGS = -I${LIBCJSON_PATH}
LIBFIDTRACK_LIBS = ${LIBFIDTRACK_PATH}/libfidtrack.a
WOSCLIB_CFLAGS = -I${WOSCLIB_PATH}
WOSCLIB_LIBS = ${WOSCLIB_PATH}/libwosclib.a

LIBMOVID_STATIC = libmovid.a

OBJ = moDataStream.o moModule.o moPipeline.o moFactory.o moProperty.o \
      moDaemon.o moLog.o moDataGenericContainer.o moDumpModule.o \
	  moCameraModule.o moImageDisplayModule.o moSmoothModule.o \
	  moImageFilterModule.o moInvertModule.o moVideoModule.o moBackgroundSubtractModule.o \
	  moGrayScaleModule.o moThresholdModule.o moAmplifyModule.o moHighpassModule.o \
	  moBlobTrackerModule.o moCombineModule.o moMirrorImageModule.o moFiducialTrackerModule.o \
	  moImageModule.o moOSC.o moTuioModule.o

#where the source is, and where to put the object files
SRC_DIR = src
MOD_DIR = ${SRC_DIR}/modules
BIN_DIR = bin

# compiler flags
CFLAGS ?= -O0 -g -Wall -I${CONTRIB_PATH}
LIBS   ?=

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS   ?= `pkg-config --libs opencv`

ALL_CFLAGS = ${CFLAGS} ${OPENCV_CFLAGS} ${WOSCLIB_CFLAGS}
ALL_LIBS   = ${LIBS} ${OPENCV_LIBS}
ALL_LIBS_STATIC = ${LIBMOVID_STATIC} ${LIBFIDTRACK_LIBS} ${WOSCLIB_LIBS}

BIN = $(addprefix ${BIN_DIR}/, ${OBJ})

all: tracker describe daemon

#rules for building targets
static: ${BIN}
	${AR} rcs ${LIBMOVID_STATIC} ${BIN}

tracker: contribs static src/tracker.cpp
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TRACKER_BIN} src/tracker.cpp ${ALL_LIBS_STATIC}

describe: contribs static src/describe.cpp
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${DESCRIBE_BIN} src/describe.cpp ${ALL_LIBS_STATIC}

blobtrack: static src/blobtracker.cpp
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${BLOB_BIN} src/blobtracker.cpp
	
daemon: contribs static src/daemon.cpp
	${CXX}   -o ${DAEMON_BIN} src/daemon.cpp contrib/cJSON/cJSON.c ${ALL_LIBS_STATIC} \
		${LIBEVENT_CFLAGS} ${LIBCJSON_CFLAGS} ${LIBJPEG_CFLAGS} ${ALL_CFLAGS} \
		${LIBJPEG_LIBS} ${LIBEVENT_LIBS} ${ALL_LIBS}

contribs:
	${MAKE} -C contrib

doc:
	${MAKE} -C docs

# how to build stuff in SRC_DIR
${BIN_DIR}/%.o : ${SRC_DIR}/%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

# how to build stuff in MOD_DIR
${BIN_DIR}/%.o : ${MOD_DIR}/%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

clean:
	-rm ${BIN_DIR}/*.o
	-rm ${TRACKER_BIN} ${DESCRIBE_BIN} ${BLOB_BIN}
	-rm ${DAEMON_BIN}
	-rm -r *.dSYM build
	-rm ${LIBMOVID_STATIC}

distclean: clean
	${MAKE} -C contrib clean
