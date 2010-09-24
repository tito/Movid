LIBEVENT_PATH     = libevent-1.4.13-stable
LIBEVENT_LIB     ?= $(LIBEVENT_PATH)/.libs/libevent.a

LIBFIDTRACK_PATH  = libfidtrack
LIBFIDTRACK_LIB  ?= $(LIBFIDTRACK_PATH)/libfidtrack.a

WOSCLIB_PATH      = WOscLib-00.09
WOSCLIB_LIB      ?= $(WOSCLIB_PATH)/libwosclib.a

PTYPES_PATH       = ptypes-2.1.1
PTYPES_LIB       ?= $(PTYPES_PATH)/lib/libptypes.a

LIBVPX_PATH       = libvpx-0.9.2
LIBVPX_LIB       ?= $(VPX_PATH)/libvpx.a


all: $(LIBEVENT_LIB) $(LIBFIDTRACK_LIB) $(WOSCLIB_LIB) $(PTYPES_LIB) $(LIBVPX
	mkdir -p ./lib
	cp $? ./lib

$(LIBEVENT_LIB):
	-cd $(LIBEVENT_PATH); test ! -f config.status && sh ./configure --disable-shared
	$(MAKE) -C $(LIBEVENT_PATH)

$(LIBFIDTRACK_LIB):
	$(MAKE) -C $(LIBFIDTRACK_PATH)

$(WOSCLIB_LIB):
	-cd $(WOSCLIB_PATH); test ! -f config.status && sh ./configure
	$(MAKE) -C $(WOSCLIB_PATH)

$(PTYPES_LIB):
	$(MAKE) -C $(PTYPES_PATH)

$(LIBVPX_LIB):
	-cd $(LIBVPX_PATH); test ! -f config.status && sh ./configure --disable-vp8-decoder --disable-examples
	$(MAKE) -C $(LIBVPX_LIB)

clean:
	-$(MAKE) -C $(LIBEVENT_PATH) distclean
	-$(MAKE) -C $(WOSCLIB_PATH) clean
	-$(MAKE) -C $(PTYPES_PATH) clean
	-$(MAKE) -C $(LIBFIDTRACK_PATH) clean
	-$(MAKE) -C $(LIBVPX_PATH) clean
	-rm -r ./lib
