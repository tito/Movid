INCLUDE=-I../include 
BIN=../bin
CFLAGS= -O2 -fomit-frame-pointer -Wall -fPIC
DEFINES=-DHAVE_CONFIG_H -D_SERVER_VERSION="\"libwebserver v0.5.3\""
LIBS=
LIBDIR=

OBJS= clientinfo.o server.o client.o gethandler.o socket.o memory.o outstream.o weblog.o utils.o fnmatch.o outgif.o error.o

all: $(OBJS)
	#$(CC) $(OBJS) $(LIBS) $(LIBDIR) -shared -o libwebserver.so
	ar -r libwebserver.a *.o
	ranlib libwebserver.a   
	mkdir -p $(BIN);
	cp libwebserver.a $(BIN)



.c.o:
	$(CC) -c $< $(CFLAGS) $(DEFINES) $(INCLUDE)





clean:
	@echo "Cleaning...";	
	@rm -f *.o 
	@rm -f libwebserver.so
	@rm -f libwebserver.a
	@rm -rf $(BIN)

distclean: clean
	@rm -f Makefile
	
	
		 
		 
