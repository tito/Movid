
## BUILDING THE LIBRARY #######################################################

To build the library run

    ./configure
    make

Build process tested under mingw/MSYS, darwin, debian etch.

## BUILDING THE DOCUMENTATION #################################################

There is a doxygen file in the ./build/doxygen directory. Simply run doxygen
on this file.

## EXAMPLES ###################################################################

This distribution contains some examples which show some applications of the
library.

## WOscServer ##

A simple UDP-based OSC server with a small address space.

woscserver [local port]

Address space:
/hello      Prints a "Hello World!" message plus all message parameters.
/echo       Echos the message to its origin (which can be additionally
            controlled by an integer parameter for the port and an string
            for the IP where the message should be echoed to.
/exit       Quits woscserver.

## WOscClient ##

A simple UDP-based OSC client/server which can send OSC messages to any UDP
based OSC server. The messages can be send including integer, string and
floating-point parameters. The remote OSC server can be set as arguments or
at runtime. The local UDP port can be changed at runtime:

woscclient [local port] [remote port] [remote IP]

Commands controlling "woscclient":

exit        terminate "woscclient".
slp [port]  set local UDP port to [port]
srp [port]  set remote server UDP destination port to [port].
srip [ip]   set remote server UDP destination IP to [ip].
	        whereas [ip] is an IP string in dotted decimal notation
            (127.0.0.1).

/[msg] [params]
            send an OSC message "/msg" to remote OSC server with
            parameters [params]. [params] can be a space separated
            list containing integers ( 666 ), floats ( 3.14 ) and
            strings ( hello_world ).

## IDEAS ######################################################################

- Reference counting for Messages, Blobs, Strings etc.
- Tests

## HISTORY ####################################################################

See SVN revision log
