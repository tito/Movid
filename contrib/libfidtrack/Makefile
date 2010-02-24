SOURCES = fidtrack120.c fidtrackX.c segment.c threshold.c topologysearch.c tiled_bernsen_threshold.c treeidmap.cpp
OBJECTS = $(SOURCES:.c=.o)
CXXFLAGS = -Wall -O2 -finline-functions -ffast-math -funroll-all-loops -fomit-frame-pointer

all: libfidtrack.a

libfidtrack.a: $(OBJECTS)
	${AR} rcs libdiftrack.a ${OBJECTS}

%.o : %.c
	$(CXX) -c $< -o $@

clean:
	-rm *.o *.a
