######################################################################
#
# Copyright (C) 2010 Movid Authors.  All rights reserved.
#
# This file is part of the Movid Software.
#
# This file may be distributed under the terms of the Q Public License
# as defined by Trolltech AS of Norway and appearing in the file
# LICENSE included in the packaging of this file.
#
# This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
# WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# Contact info@movid.org if any conditions of this licensing are
# not clear to you.
#
######################################################################

#
# Configuration
#
MOVID_BIN			= movid
MOVID_LIB			= libmovid.a
CONTRIB_PATH		= contrib

#
# Contribs
#
LIBEVENT_PATH		= $(CONTRIB_PATH)/libevent-1.4.13-stable
LIBCJSON_PATH 		= $(CONTRIB_PATH)/cJSON
LIBFIDTRACK_PATH 	= $(CONTRIB_PATH)/libfidtrack
WOSCLIB_PATH 		= $(CONTRIB_PATH)/WOscLib-00.09
PTYPES_PATH 		= $(CONTRIB_PATH)/ptypes-2.1.1

#
# Source files
#
SOURCES = \
	src/moDaemon.cpp \
	src/moDataGenericContainer.cpp \
	src/moDataStream.cpp \
	src/moFactory.cpp \
	src/moLog.cpp \
	src/moModule.cpp \
	src/moOSC.cpp \
	src/moPipeline.cpp \
	src/moProperty.cpp \
	src/moThread.cpp \
	src/moUtils.cpp \
	src/modules/moAmplifyModule.cpp \
	src/modules/moBackgroundSubtractModule.cpp \
	src/modules/moBlobTrackerModule.cpp \
	src/modules/moBlobTrackerKNNModule.cpp \
	src/modules/moCameraModule.cpp \
	src/modules/moCannyModule.cpp \
	src/modules/moCombineModule.cpp \
	src/modules/moFingerTipFinderModule.cpp \
	src/modules/moDilateModule.cpp \
	src/modules/moDistanceTransformModule.cpp \
	src/modules/moDumpModule.cpp \
	src/modules/moErodeModule.cpp \
	src/modules/moFiducialTrackerModule.cpp \
	src/modules/moGrayScaleModule.cpp \
	src/modules/moHighpassModule.cpp \
	src/modules/moHsvModule.cpp \
	src/modules/moImageDisplayModule.cpp \
	src/modules/moImageFilterModule.cpp \
	src/modules/moImageModule.cpp \
	src/modules/moInvertModule.cpp \
	src/modules/moMaskModule.cpp \
	src/modules/moMirrorImageModule.cpp \
	src/modules/moPeakFinderModule.cpp \
	src/modules/moRoiModule.cpp \
	src/modules/moSmoothModule.cpp \
	src/modules/moThresholdModule.cpp \
	src/modules/moTuioModule.cpp \
	src/modules/moVideoModule.cpp \
	src/modules/moYCrCbThresholdModule.cpp \
	#AUTOMODULE_DoNotRemoveThisComment


#
# Compiler flags
#
target				?= debug
ifeq ($(target),release)
CFLAGS_TARGET		?= -O2 -DNDEBUG -DNO_LOG
else
ifeq ($(target),debug-optimized)
CFLAGS_TARGET 		?= -O2 -ggdb
else
CFLAGS_TARGET 		?= -O0 -ggdb
endif
endif
CFLAGS				?= $(CFLAGS_TARGET) -I$(CONTRIB_PATH) -Wall
LIBS   				?=


#
# Internal variables, to make the Makefile easier to read
#

OBJECTS				= $(SOURCES:.cpp=.o)

OPENCV_CFLAGS		?= `pkg-config --cflags opencv`
OPENCV_LIBS			?= `pkg-config --libs opencv`
LIBEVENT_LIB		?= $(LIBEVENT_PATH)/.libs/libevent.a
LIBEVENT_CFLAGS		?= -I$(LIBEVENT_PATH)
LIBCJSON_CFLAGS		?= -I$(LIBCJSON_PATH)
LIBFIDTRACK_LIB		?= $(LIBFIDTRACK_PATH)/libfidtrack.a
WOSCLIB_CFLAGS		?= -I$(WOSCLIB_PATH)
WOSCLIB_LIB			?= $(WOSCLIB_PATH)/libwosclib.a
PTYPES_CFLAGS		?= -I$(PTYPES_PATH)/include
PTYPES_LIB			?= $(PTYPES_PATH)/lib/libptypes.a

ALL_CFLAGS			= $(CFLAGS) $(OPENCV_CFLAGS) $(WOSCLIB_CFLAGS) $(PTYPES_CFLAGS)
ALL_LIBS			= $(LIBS) $(OPENCV_LIBS)
ALL_LIBS_CONTRIB	= $(LIBFIDTRACK_LIB) $(WOSCLIB_LIB) $(PTYPES_LIB)
ALL_LIBS_STATIC		= $(MOVID_LIB) $(ALL_LIBS_CONTRIB)

#
# Global rules
#

all: movid
distclean: cleandepend cleancontrib clean


#
# Dependices
#

include Makefile.depend

depend: Makefile.depend

cleandepend:
	-rm Makefile.depend

Makefile.depend: $(SOURCES)
	@touch Makefile.depend
	-makedepend -fMakefile.depend -- $(ALL_CFLAGS) $(SOURCES) 2>/dev/null
	-rm Makefile.depend.bak

#
# Contribs
#

contrib: $(LIBEVENT_LIB) $(LIBFIDTRACK_LIB) $(WOSCLIB_LIB) $(PTYPES_LIB)

cleancontrib:
	-$(MAKE) -C $(LIBEVENT_PATH) distclean
	-$(MAKE) -C $(WOSCLIB_PATH) clean
	-$(MAKE) -C $(PTYPES_PATH) clean
	-$(MAKE) -C $(LIBFIDTRACK_PATH) clean

$(LIBEVENT_LIB):
	cd $(LIBEVENT_PATH) && sh ./configure --disable-shared && $(MAKE)

$(LIBFIDTRACK_LIB):
	$(MAKE) -C $(LIBFIDTRACK_PATH)

$(WOSCLIB_LIB):
	cd $(WOSCLIB_PATH) && sh ./configure && $(MAKE) libwosclib.a

$(PTYPES_LIB):
	cd $(PTYPES_PATH) && make


#
# Movid
#

%.o: %.cpp
	$(CXX) $(ALL_CFLAGS) -c $< -o $@

clean:
	-rm $(OBJECTS) 2>/dev/null
	-rm $(MOVID_LIB) 2>/dev/null
	-rm $(MOVID_BIN) 2>/dev/null

movid: Makefile.depend contrib $(MOVID_LIB) src/movid.cpp
	$(CXX) -o $(MOVID_BIN) src/movid.cpp contrib/cJSON/cJSON.c \
		$(ALL_LIBS_STATIC) \
		$(LIBEVENT_CFLAGS) $(LIBCJSON_CFLAGS) $(ALL_CFLAGS) \
		$(LIBEVENT_LIB) $(ALL_LIBS)

$(MOVID_LIB): $(OBJECTS)
	$(AR) rcs $(MOVID_LIB) $(OBJECTS)


#
# Create a new module
#

newmodule:
	cat src/modules/.dummy.h | sed "s/MO_DUMMY/\U\MO_${NAME}/g" | sed "s/DUMMY/${NAME}/g" > src/modules/mo${NAME}Module.h
	cat src/modules/.dummy.cpp | sed "s/DUMMY/${NAME}/g" > src/modules/mo${NAME}Module.cpp
	#cat Makefile | sed "s/#AUTOMODULE_DoNotRemoveThisComment/mo${NAME}Module.o#AUTOMODULE_DoNotRemoveThisComment/" > Makefile.tmp
	cat src/moFactory.cpp | sed "s/\/\/AUTOMODULE_DoNotRemoveThisComment/REGISTER_MODULE(${NAME});\n\t\/\/AUTOMODULE_DoNotRemoveThisComment/g" > src/moFactory.cpp.tmp
	mv src/moFactory.cpp.tmp src/moFactory.cpp


#
# Help
#

help:
	@echo "List of availables target                                   "
	@echo "   all                 Build dependices and the movid binary"
	@echo "   clean               Clean movid objects                  "
	@echo "   cleandepend         Clean the dependice file             "
	@echo "   contrib             Build contribs                       "
	@echo "   depend              Build dependices                     "
	@echo "   distclean           Clean contrib + movid objects        "
	@echo "   help                You actually read it                 "
	@echo "   newmodule NAME=aa   Create a new module named aa         "
