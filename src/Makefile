OUTPUT = tracker
OBJ = otCamera.o

OPENCV_CFLAGS   ?= `pkg-config --cflags opencv`
OPENCV_LIBS     ?= `pkg-config --libs opencv`

CFLAGS = ${OPENCV_CFLAGS}
LIBS   = ${OPENCV_LIBS}   


all: ${OBJ}
	${CXX} ${LIBS} ${CFLAGS} -o ${OUTPUT} tracker.cpp ${OBJ} 	
	
%.o : %.cpp
	${CXX} ${LIBS} ${CFLAGS} -c $<
	
clean:
	rm ${OBJ} ${OUTPUT}
