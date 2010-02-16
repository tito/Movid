#stuff we need to compile
TRACKER_BIN = tracker
TESTER_BIN = tester
DESCRIBE_BIN = describe

OBJ = otDataStream.o otModule.o otPipeline.o otParser.o otFactory.o otProperty.o \
      otDaemon.o otLog.o otCameraModule.o otImageDisplayModule.o otGaussianBlurModule.o \
	  otImageFilterModule.o otInvertModule.o

#where the source is, and where to put the object files
SRC_DIR = src/
MOD_DIR = ${SRC_DIR}/modules/
BIN_DIR = bin/

# compiler flags
CFLAGS ?= -O0 -g -Wall
LIBS   ?=

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS   ?= `pkg-config --libs opencv`

ALL_CFLAGS = ${CFLAGS} ${OPENCV_CFLAGS}
ALL_LIBS   = ${LIBS} ${OPENCV_LIBS}

BIN = $(addprefix ${BIN_DIR}, ${OBJ})

#rules for building targets
all: ${BIN}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TRACKER_BIN} src/tracker.cpp ${BIN}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TESTER_BIN} src/tester.cpp ${BIN}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${DESCRIBE_BIN} src/describe.cpp ${BIN}
	
#how to build stuff in SRC_DIR
${BIN_DIR}%.o : ${SRC_DIR}%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

#how to build stuff in MOD_DIR
${BIN_DIR}%.o : ${MOD_DIR}%.cpp
	${CXX} ${ALL_CFLAGS} -c $< -o $@

clean:
	rm -r ${TESTER_BIN} ${TRACKER_BIN} ${BIN_DIR}/*.o *.dSYM build 2>/dev/null &>/dev/null
