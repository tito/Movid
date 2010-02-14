OUTPUT = tracker
OBJ = otDataStream.o otImageDataStream.o otCameraDataStream.o otModule.o otImageDisplayModule.o otGroup.o otParser.o

CFLAGS ?= -g -Wall
LIBS   ?=

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS   ?= `pkg-config --libs opencv`

ALL_CFLAGS = ${CFLAGS} ${OPENCV_CFLAGS}
ALL_LIBS   = ${LIBS} ${OPENCV_LIBS}   


all: ${OBJ}
	${CXX} ${ALL_LIBS} ${ALL_CFLAGS} -o ${OUTPUT} tracker.cpp ${OBJ} 	
	
%.o : %.cpp
	${CXX} ${ALL_CFLAGS} -c $<
	
clean:
	rm -r ${OUTPUT} *.o *.dSYM
