OUTPUT = tracker

OPENCV_CFLAGS ?= `pkg-config --cflags opencv`
OPENCV_LIBS ?= `pkg-config --libs opencv`
CFLAGS ?= ${OPENCV_CFLAGS}
LIBS ?= ${OPENCV_LIBS}

all:
	cd src && gcc ${LIBS} ${CFLAGS} -o ${OUTPUT} tracker.cpp
