# contrib path
CONTRIB_PATH = contrib
LIBEVENT_PATH = ${CONTRIB_PATH}/libevent-1.4.13-stable
LIBCJSON_PATH = ${CONTRIB_PATH}/cJSON
LIBJPEG_PATH = ${CONTRIB_PATH}/jpeg-8

#stuff we need to compile
TRACKER_BIN = tracker
TESTER_BIN = tester
BLOB_BIN = blobtrack
DESCRIBE_BIN = describe
DAEMON_BIN = daemon

LIBEVENT_LIBS = ${LIBEVENT_PATH}/.libs/libevent.a
LIBEVENT_CFLAGS = -I${LIBEVENT_PATH}
LIBJPEG_LIBS = ${LIBJPEG_PATH}/.libs/libjpeg.a
LIBJPEG_CFLAGS = -I${LIBJPEG_PATH}
LIBCJSON_CFLAGS = -I${LIBCJSON_PATH}

LIBOT_STATIC = libot.a

OBJ = otDataStream.o otModule.o otPipeline.o otParser.o otFactory.o otProperty.o \
      otDaemon.o otLog.o otDataGenericContainer.o otDumpModule.o \
	  otCameraModule.o otImageDisplayModule.o otSmoothModule.o \
	  otImageFilterModule.o otInvertModule.o otVideoModule.o otBackgroundSubtractModule.o \
	  otGrayScaleModule.o otThresholdModule.o otAmplifyModule.o otHighpassModule.o \
	  otBlobTrackerModule.o otCombineModule.o otMirrorImageModule.o

#where the source is, and where to put the object files
SRC_DIR = src
MOD_DIR = ${SRC_DIR}/modules
BIN_DIR = bin

# compiler flags
CFLAGS ?= -O0 -g -Wall
LIBS   ?=

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS   ?= `pkg-config --libs opencv`

ALL_CFLAGS = ${CFLAGS} ${OPENCV_CFLAGS}
ALL_LIBS   = ${LIBS} ${OPENCV_LIBS}
ALL_LIBS_STATIC = ${LIBOT_STATIC}

BIN = $(addprefix ${BIN_DIR}/, ${OBJ})

#rules for building targets
all: static daemon
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TRACKER_BIN} src/tracker.cpp ${LIBOT_STATIC}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TESTER_BIN} src/tester.cpp ${LIBOT_STATIC}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${BLOB_BIN} src/blobtracker.cpp
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${DESCRIBE_BIN} src/describe.cpp ${LIBOT_STATIC}
	
static: ${BIN}
	${AR} rcs ${LIBOT_STATIC} ${BIN}

daemon: contribs static src/daemon.cpp
	${CXX}   -o ${DAEMON_BIN} src/daemon.cpp contrib/cJSON/cJSON.c ${ALL_LIBS_STATIC} \
		${LIBEVENT_CFLAGS} ${LIBCJSON_CFLAGS} ${LIBJPEG_CFLAGS} ${ALL_CFLAGS} \
		${LIBJPEG_LIBS} ${LIBEVENT_LIBS} ${ALL_LIBS} \

contribs:
	${MAKE} -C contrib

# how to build stuff in SRC_DIR
${BIN_DIR}/%.o : ${SRC_DIR}/%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

# how to build stuff in MOD_DIR
${BIN_DIR}/%.o : ${MOD_DIR}/%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

clean:
	-rm ${BIN_DIR}/*.o
	-rm ${TESTER_BIN} ${TRACKER_BIN} ${DESCRIBE_BIN} ${BLOB_BIN}
	-rm ${DAEMON_BIN}
	-rm -r *.dSYM build
	-rm ${LIBOT_STATIC}

distclean: clean
	${MAKE} -C contrib clean
