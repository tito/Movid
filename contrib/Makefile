LIBEVENT_PATH     = libevent-1.4.13-stable
LIBEVENT_LIB     ?= $(LIBEVENT_PATH)/.libs/libevent.a

LIBFIDTRACK_PATH  = libfidtrack
LIBFIDTRACK_LIB  ?= $(LIBFIDTRACK_PATH)/libfidtrack.a

WOSCLIB_PATH      = WOscLib-00.09
WOSCLIB_LIB      ?= $(WOSCLIB_PATH)/libwosclib.a

PTYPES_PATH       = ptypes-2.1.1
PTYPES_LIB       ?= $(PTYPES_PATH)/lib/libptypes.a


all: $(LIBEVENT_LIB) $(LIBFIDTRACK_LIB) $(WOSCLIB_LIB) $(PTYPES_LIB)
	mkdir -p ./lib
	cp $? ./lib

$(LIBEVENT_LIB):
	-cd $(LIBEVENT_PATH); test -f config.status && sh ./configure --disable-shared
	$(MAKE) -C (LIBEVENT_PATH)

$(LIBFIDTRACK_LIB):
	$(MAKE) -C $(LIBFIDTRACK_PATH)

$(WOSCLIB_LIB):
	-cd $(WOSCLIB_PATH); test -f config.status && sh ./configure
	$(MAKE) -C $(WOSCLIB_PATH)

$(PTYPES_LIB):
	$(MAKE) -C $(PTYPES_PATH)

clean:
	-$(MAKE) -C $(LIBEVENT_PATH) distclean
	-$(MAKE) -C $(WOSCLIB_PATH) clean
	-$(MAKE) -C $(PTYPES_PATH) clean
	-$(MAKE) -C $(LIBFIDTRACK_PATH) clean
	-rm -r ./lib