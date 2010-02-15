TRACKER_BIN = tracker
TESTER_BIN = tester

OBJ = otDataStream.o otCamera.o otModule.o otImageDisplayModule.o otGroup.o otParser.o otFactory.o otDaemon.o otGaussianBlurModule.o

CFLAGS ?= -O0 -g -Wall
LIBS   ?=

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS   ?= `pkg-config --libs opencv`

ALL_CFLAGS = ${CFLAGS} ${OPENCV_CFLAGS}
ALL_LIBS   = ${LIBS} ${OPENCV_LIBS}


all: ${OBJ}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TRACKER_BIN} tracker.cpp ${OBJ}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${TESTER_BIN} tester.cpp ${OBJ}
	
%.o : %.cpp
	${CXX} ${ALL_CFLAGS} -c $<
	
clean:
	rm -r ${OUTPUT} *.o *.dSYM   #-r becasue .dSYM are directories made by XCode
