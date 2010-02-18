# contrib path
CONTRIB_PATH = contrib
LIBWEBSERVER_PATH = ${CONTRIB_PATH}/libwebserver-0.5.3

#stuff we need to compile
TRACKER_BIN = tracker
TESTER_BIN = tester
BLOB_BIN = blobtrack
DESCRIBE_BIN = describe
DAEMON_BIN = daemon

LIBWEBSERVER_LIBS = ${LIBWEBSERVER_PATH}/bin/libwebserver.a
LIBWEBSERVER_CFLAGS = -I${LIBWEBSERVER_PATH}/include

LIBOT_STATIC = libot.a
LIBOT_SHARED = libot.so

SDLGUI_BIN = otgui_sdl
SDLGUI_LIBS = `pkg-config --libs --cflags sdl` `pkg-config --libs --cflags SDL_gfx`

OBJ = otDataStream.o otModule.o otPipeline.o otParser.o otFactory.o otProperty.o \
      otDaemon.o otLog.o otDataGenericContainer.o otDumpModule.o \
	  otCameraModule.o otImageDisplayModule.o otSmoothModule.o \
	  otImageFilterModule.o otInvertModule.o otVideoModule.o otBackgroundSubtractModule.o \
	  otGrayScaleModule.o otThresholdModule.o otAmplifyModule.o otHighpassModule.o \
	  otBlobTrackerModule.o otCombineModule.o

#where the source is, and where to put the object files
SRC_DIR = src
MOD_DIR = ${SRC_DIR}/modules
BIN_DIR = bin

# compiler flags
CFLAGS ?= -O0 -g -Wall # -fPIC << needed for shared
LIBS   ?=

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS   ?= `pkg-config --libs opencv`

ALL_CFLAGS = ${CFLAGS} ${OPENCV_CFLAGS} ${LIBWEBSERVER_CFLAGS}
ALL_LIBS   = ${LIBS} ${OPENCV_LIBS} ${LIBWEBSERVER_LIBS}
ALL_LIBS_STATIC = ${LIBOT_STATIC} ${LIBWEBSERVER_LIBS}

BIN = $(addprefix ${BIN_DIR}/, ${OBJ})

#rules for building targets
all: static libwebserver daemon
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TRACKER_BIN} src/tracker.cpp ${LIBOT_STATIC}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TESTER_BIN} src/tester.cpp ${LIBOT_STATIC}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${BLOB_BIN} src/blobtracker.cpp
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${DESCRIBE_BIN} src/describe.cpp ${LIBOT_STATIC}
	

static: ${BIN}
	${AR} rcs ${LIBOT_STATIC} ${BIN}

gui: static
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} ${SDLGUI_LIBS} -o ${SDLGUI_BIN} src/sdlgui.cpp ${LIBOT_STATIC}

daemon: src/daemon.cpp static libwebserver
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${DAEMON_BIN} src/daemon.cpp ${ALL_LIBS_STATIC} 

libwebserver:
	cd ${LIBWEBSERVER_PATH}/src; make


#shared: ${BIN}
#	${CXX} -shared -Wl,-soname,${LIBOT_SHARED} -o ${LIBOT_SHARED} ${BIN}

#how to build stuff in SRC_DIR
${BIN_DIR}/%.o : ${SRC_DIR}/%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

#how to build stuff in MOD_DIR
${BIN_DIR}/%.o : ${MOD_DIR}/%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

clean:
	-rm  ${BIN_DIR}/*.o &>/dev/null
	-rm ${TESTER_BIN} ${TRACKER_BIN} ${DESCRIBE_BIN} ${BLOB_BIN} ${SDLGUI_BIN} &>/dev/null
	-rm ${DAEMON_BIN} &>/dev/null
	-rm -r  *.dSYM build  &>/dev/null
	-rm ${LIBOT_STATIC} ${LIBOT_SHARED} &>/dev/null
	cd ${LIBWEBSERVER_PATH}/src; make clean
