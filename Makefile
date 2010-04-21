# contrib path
CONTRIB_PATH = contrib
LIBEVENT_PATH = ${CONTRIB_PATH}/libevent-1.4.13-stable
LIBCJSON_PATH = ${CONTRIB_PATH}/cJSON
LIBJPEG_PATH = ${CONTRIB_PATH}/jpeg-8
LIBFIDTRACK_PATH = ${CONTRIB_PATH}/libfidtrack
WOSCLIB_PATH = ${CONTRIB_PATH}/WOscLib-00.09
PTYPES_PATH = ${CONTRIB_PATH}/ptypes-2.1.1

#stuff we need to compile
BLOB_BIN = blobtrack
MOVID_BIN = movid

LIBEVENT_LIBS = ${LIBEVENT_PATH}/.libs/libevent.a
LIBEVENT_CFLAGS = -I${LIBEVENT_PATH}
LIBJPEG_LIBS = ${LIBJPEG_PATH}/.libs/libjpeg.a
LIBJPEG_CFLAGS = -I${LIBJPEG_PATH}
LIBCJSON_CFLAGS = -I${LIBCJSON_PATH}
LIBFIDTRACK_LIBS = ${LIBFIDTRACK_PATH}/libfidtrack.a
WOSCLIB_CFLAGS = -I${WOSCLIB_PATH}
WOSCLIB_LIBS = ${WOSCLIB_PATH}/libwosclib.a
PTYPES_CFLAGS = -I${PTYPES_PATH}/include
PTYPES_LIB = ${PTYPES_PATH}/lib/libptypes.a

LIBMOVID_STATIC = libmovid.a

OBJ = moThread.o moDataStream.o moModule.o moPipeline.o moFactory.o moProperty.o \
      moDaemon.o moLog.o moDataGenericContainer.o moDumpModule.o moUtils.o \
      moCameraModule.o moImageDisplayModule.o moSmoothModule.o moRoiModule.o \
      moImageFilterModule.o moInvertModule.o moVideoModule.o moBackgroundSubtractModule.o \
      moGrayScaleModule.o moThresholdModule.o moAmplifyModule.o moHighpassModule.o \
      moBlobTrackerModule.o moCombineModule.o moMirrorImageModule.o moFiducialTrackerModule.o \
      moImageModule.o moOSC.o moTuioModule.o moDilateModule.o moErodeModule.o \
      moCannyModule.o moCalibrationModule.o
      #DoNotRemoveThisComment

#where the source is, and where to put the object files
SRC_DIR = src
MOD_DIR = ${SRC_DIR}/modules
BIN_DIR = bin

# compiler flags
CFLAGS ?= -O0 -g -Wall -I${CONTRIB_PATH}
LIBS   ?=

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS   ?= `pkg-config --libs opencv`

ALL_CFLAGS = ${CFLAGS} ${OPENCV_CFLAGS} ${WOSCLIB_CFLAGS} ${PTYPES_CFLAGS}
ALL_LIBS   = ${LIBS} ${OPENCV_LIBS}
ALL_LIBS_STATIC = ${LIBMOVID_STATIC} ${LIBFIDTRACK_LIBS} ${WOSCLIB_LIBS} ${PTYPES_LIB}

BIN = $(addprefix ${BIN_DIR}/, ${OBJ})

all: movid

newmodule:
	cat src/modules/.dummy.h | sed "s/MO_DUMMY/MO_${NAME}/g" | sed "s/DUMMY/${NAME}/g" > src/modules/mo${NAME}Module.h
	cat src/modules/.dummy.cpp | sed "s/DUMMY/${NAME}/g" > src/modules/mo${NAME}Module.cpp
	#cat Makefile | sed "s/#DoNotRemoveThisComment/mo${NAME}Module.o#DoNotRemoveThisComment/g" > Makefile
	cat src/moFactory.cpp | sed "s/\/\/DoNotRemoveThisComment/REGISTER_MODULE(${NAME});\/\/DoNotRemoveThisComment/g" > src/moFactory.cpp


#rules for building targets
static: ${BIN}
	${AR} rcs ${LIBMOVID_STATIC} ${BIN}

blobtrack: static src/blobtracker.cpp
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${BLOB_BIN} src/blobtracker.cpp
	
movid: contribs static src/movid.cpp
	${CXX} -o ${MOVID_BIN} src/movid.cpp contrib/cJSON/cJSON.c ${ALL_LIBS_STATIC} \
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
	-rm ${BLOB_BIN}
	-rm ${MOVID_BIN}
	-rm -r *.dSYM build
	-rm ${LIBMOVID_STATIC}

distclean: clean
	${MAKE} -C contrib clean
