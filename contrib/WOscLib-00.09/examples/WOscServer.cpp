#include "config.h"

///////////////////////////////////////////////////////////////////////////////
// OS dependent includes
///////////////////////////////////////////////////////////////////////////////
   
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
#	include <unistd.h>			//	usleep
#	include <fcntl.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#	include <sys/socket.h>
#elif OS_IS_WIN32 == 1
#	include "windows.h"
#	include "winsock2.h"
#	define socklen_t	int
#else
#	error "Invalid Platform"
#endif

///////////////////////////////////////////////////////////////////////////////
// general includes
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "WOscReceiver.h"
#include "WOscNetReturn.h"
#include "WOscReceiverMethod.h"

#include "WOscServer.h"

///////////////////////////////////////////////////////////////////////////////
// defines
///////////////////////////////////////////////////////////////////////////////
   
#define OSC_SERVER_PORT			10000
#define PROTOCOL_UDP			17
#define WOS_MAX_RX_UDP_PACKET	2048

#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
socklen_t WOS_SIZE_NRA = sizeof(sockaddr_in);
#elif OS_IS_WIN32 == 1
int WOS_SIZE_NRA = sizeof(sockaddr_in);
#endif

///////////////////////////////////////////////////////////////////////////////
// WOscServerMethod
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Initializes WOscReceiverMethod base class. */
WOscServerMethod::WOscServerMethod(
	WOscContainer* parent,
	WOscServer* receiverContext,
	const char* methodName,
	const char* methodDescription)
:WOscReceiverMethod(
	parent,
	(WOscReceiver*)receiverContext,
	methodName,
	methodDescription)
{}

/** Type-save access to receiver object, which is owning this object.        */
WOscServer* WOscServerMethod::GetContext()
{
	return (WOscServer*)WOscReceiverMethod::GetContext();
}

/** Type-save method interface. All methods inherited from this class get
 * the correct TheNetReturnAddress pointers without caring about the
 * system below.
 */
void WOscServerMethod::Method(
	const WOscMessage *message,
	const WOscTimeTag& when,
	const WOscNetReturn* networkReturnAddress)
{
	Method(message, when, (TheNetReturnAddress*)networkReturnAddress);
}

///////////////////////////////////////////////////////////////////////////////
// TheOscHelloMethod
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Sets the method name and info in the base class.            */
TheOscHelloMethod::TheOscHelloMethod(
	WOscContainer* parent,
	WOscServer* receiverContext)
:WOscServerMethod(
	parent,
	receiverContext,
	"hello",
	"A hello word method.")
{}

/** The hello method. Prints "Hello World!" and the contents of the message
 * on the console.
 */
void TheOscHelloMethod::Method(
	const WOscMessage *message,
	const WOscTimeTag& when,
	const TheNetReturnAddress* networkReturnAddress)
{
	std::cout << "Hello World! This message contains:\n" ;
	int nStr = message->GetNumStrings();
	int nInt = message->GetNumInts();
	int nFlt = message->GetNumFloats();
	for (int i = 0; i < nStr; i++ )
		std::cout << "  str["<<i<<"]\t" << 
			message->GetString(i).GetBuffer() << "\n" ;
	for (int i = 0; i < nInt; i++ )
		std::cout << "  int["<<i<<"]\t" << 
			message->GetInt(i) << "\n" ;
	for (int i = 0; i < nFlt; i++ )
		std::cout << "  flt["<<i<<"]\t" << 
			message->GetFloat(i) << "\n" ;
}

///////////////////////////////////////////////////////////////////////////////
// TheOscExitMethod
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Sets the method name and info in the base class.            */
TheOscExitMethod::TheOscExitMethod(
	WOscContainer* parent,
	WOscServer* receiverContext)
:WOscServerMethod(
	parent,
	receiverContext,
	"exit",
	"This method quits WOscServer")
{}

/** The exit method. Terminates the WOscServer by setting the exit flag.     */
void TheOscExitMethod::Method(
	const WOscMessage *message,
	const WOscTimeTag& when,
	const TheNetReturnAddress* networkReturnAddress)
{
	GetContext()->SetExit();
}

///////////////////////////////////////////////////////////////////////////////
// TheOscEchoMethod
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Sets the method name and info in the base class.            */
TheOscEchoMethod::TheOscEchoMethod(
	WOscContainer* parent,
	WOscServer* receiverContext)
:WOscServerMethod(
	parent,
	receiverContext,
	"echo",
	"This method quits WOscServer")
{}

/** The echo method. Echoes the message back to its origin. The "origin" can be
 * modified by passing an integer and string parameter with the message. The
 * integer modifies the return port and the string the return IP.
 */
void TheOscEchoMethod::Method(
	const WOscMessage *message,
	const WOscTimeTag& when,
	const TheNetReturnAddress* networkReturnAddress)
{
	WOscMessage echoMsg = *message;
	
	TheNetReturnAddress ra = *networkReturnAddress;
	
	if ( message->GetNumInts() > 0 )
		ra.m_addr.sin_port           = htons (message->GetInt(0));
	if ( message->GetNumStrings() > 0 )
		ra.m_addr.sin_addr.s_addr    = inet_addr(message->GetString(0).GetBuffer());
	
	std::cout << "Echo to:\n" 
		<< "Port: " << ntohs(ra.m_addr.sin_port) << "\n"
		<< "IP:   " << inet_ntoa( ra.m_addr.sin_addr ) << "\n";
		
	GetContext()->NetworkSend(
		echoMsg.GetBuffer(),
		echoMsg.GetBufferLen(),
		&ra);		
}

///////////////////////////////////////////////////////////////////////////////
// WOscServer
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Sets up the OSC address space, allocates an UDP receive buffer
 * and initializes the exit flag to "false".
 */
WOscServer::WOscServer()
{
	// setup OSC address space //

	// containers
	WOscContainerInfo rootInfo("root");
	WOscContainerInfo etcInfo("etc");
	WOscContainer* rootContainer =
		new WOscContainer( &rootInfo );
	WOscContainer* etcContainer =
		new WOscContainer( &etcInfo, rootContainer, "etc" );
	
	// "root" methods
	new TheOscHelloMethod( rootContainer, this );
	new TheOscExitMethod( rootContainer, this );
	new TheOscEchoMethod( rootContainer, this );
	
	// "etc" methods
	new TheOscHelloMethod( etcContainer, this );

	SetAddressSpace(rootContainer);

	// allocate RX buffer
    m_rxbuffer = new char[WOS_MAX_RX_UDP_PACKET];

	m_exit = false;
}

/** Destructor. Removes the address space and deallocates the UDP receive
 * buffer.
 */
WOscServer::~WOscServer()
{
	// remove address space //
	WOscContainer* as = GetAddressSpace();
	if ( as ) as->RemoveAll();
}

/** Checks the exit flag.
 * \returns
 * True if application/serve exit is requested.
 */
bool WOscServer::Exit() const
{
	return m_exit;
}

/** Sets the exit flag, which is used to check for server termination.       */
void WOscServer::SetExit()
{
	m_exit = true;
}

/** Sends data over the network.
 * \param data
 * Pointer to buffer containing the data to be sent.
 *
 * \param dataLen
 * Number of bytes in the data buffer.
 *
 * \param networkReturnAddress
 * The destination network address.
 */
void WOscServer::NetworkSend(
	const char* data,
	int dataLen,
	const WOscNetReturn* networkReturnAddress)
{
	const TheNetReturnAddress* nra = (const TheNetReturnAddress*)networkReturnAddress;
	int actSend = sendto(
		m_hSocket,
		data,
		dataLen,
		0,
		(struct sockaddr*)&nra->m_addr,
		WOS_SIZE_NRA ) ;
	
	// check if transmission was successful
	if ( dataLen != actSend )
		std::cout << "Error sending packet.\n" ;
}

/** Callback for OSC packets which have caused an exception in the OSC 
 * receiver.
 * \param data
 * Pointer to buffer containing the packet data.
 * \param dataLen
 * Length of the packet data in the buffer.
 * \exception
 * Exception that caused this handler to be called.
 */
void WOscServer::HandleOffendingPackets(
	const char* const data,
	int dataLen,
	WOscException* exception)
{
	std::cout<<"HandleOffendingPackets.\n";
}

/** Callback for messages not matching any address in the local OSC address
 * space.
 * \param msg
 * Message which hasn't been processed.
 * \param networkReturnAddress
 * Network origin of the message.
 */
void WOscServer::HandleNonmatchedMessages(
	const WOscMessage* msg,
	const WOscNetReturn* networkReturnAddress)
{
	std::cout<<"HandleNonmatchedMessages.\n";
}

/** Initializes the underlying UDP network layer.
 * \param port
 * Port at which WOscServer should listen for OSC data.
 * \returns
 * Error code on error, WOS_ERR_NO_ERROR on success.
 */
WOscServer::WOscServerErrors WOscServer::NetworkInit(int port)
{
	int err;
	const int REUSE_TRUE = 1, BROADCAST_TRUE = 1;
	
#if OS_IS_WIN32 == 1
	// fucking windows winsock startup
	WSADATA wsa;
	err = WSAStartup(MAKEWORD(2,0),&wsa);
 	if ( err != 0 )
	{
		std::cout << "Error starting Windows socket subsystem.\n" ;
		return WOS_ERR_SOCKET_WSASTART;
	}
#endif	// #if OS_IS_WIN32 == 1

	// create socket
	m_hSocket = socket (AF_INET, SOCK_DGRAM, PROTOCOL_UDP);
	if (m_hSocket < 0)
	{
		std::cout << "Create socket error.\n";
		return WOS_ERR_SOCKET_CREATE;
	}
	
	// initialize server address to localhost:port
	m_serverAddr.m_addr.sin_family         = AF_INET;
	m_serverAddr.m_addr.sin_addr.s_addr    = htonl (INADDR_ANY);
	m_serverAddr.m_addr.sin_port           = htons (port);
	
	// set socket to reuse the address
	err = setsockopt(
		m_hSocket,
		SOL_SOCKET,
		SO_REUSEADDR,
		(const char*)&REUSE_TRUE,
		sizeof(REUSE_TRUE));
	if ( err != 0 )
	{
		std::cout << "Error setting socket reuse.\n" ;
		return WOS_ERR_SOCKET_REUSE;
	}
	// enable broadcasting for this socket
	setsockopt(
		m_hSocket,
		SOL_SOCKET,
		SO_BROADCAST,
		(const char*)&BROADCAST_TRUE,
		sizeof(BROADCAST_TRUE));
	if ( err != 0 )
	{
		std::cout << "Error setting socket broadcast.\n" ;
		return WOS_ERR_SOCKET_BROADCAST;
	}
	
	// disable blocking, polling is used in this example.
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
	err = fcntl(m_hSocket, F_SETFL, O_NONBLOCK);
#elif OS_IS_WIN32 == 1
	// disable blocking (for this example)
	unsigned long val = 1;
	err = ioctlsocket(m_hSocket, FIONBIO , &val);
#endif
	if ( err != 0 )
	{
		std::cout << "Error setting socket unblock.\n" ;
		return WOS_ERR_SOCKET_BLOCK;
	}
	
	// bind for listening
	err = bind (
		m_hSocket,
		(struct sockaddr *)&m_serverAddr.m_addr,
		WOS_SIZE_NRA );
	if ( err != 0 )
	{
		std::cout << "Error socket bind.\n" ;
		return WOS_ERR_SOCKET_BIND;
	}

	return WOS_ERR_NO_ERROR;

}

/** Stops the network interface.
 * \returns
 * WOS_ERR_NO_ERROR on success.
 */
WOscServer::WOscServerErrors WOscServer::NetworkHalt()
{
	// close socket... 
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
	close(m_hSocket);
#elif OS_IS_WIN32 == 1
	closesocket(m_hSocket);
	WSACleanup();
#endif
	return WOS_ERR_NO_ERROR;
}

/** Check if there is an UDP packet. On packet arrival it will be passed 
 * to the OSC receiver. It returns if there are no more packets available.
 * \returns
 * WOS_ERR_NO_ERROR if no error occurred.
 */
WOscServer::WOscServerErrors WOscServer::CheckForPackets()
{
	// as long there are packets waiting
    bool morePackets = 1;
    while(morePackets)
    {
		// has to be allocated on heap, because is reference
		// counted
		TheNetReturnAddress* nra = new TheNetReturnAddress;
		nra->m_addr.sin_family         = AF_INET;
		nra->m_addr.sin_addr.s_addr    = htonl (INADDR_ANY);
		nra->m_addr.sin_port           = htons (0);
	
		// receive from network
		int nReceived = recvfrom(
			m_hSocket,
			m_rxbuffer,
			WOS_MAX_RX_UDP_PACKET,
			0,
			(struct sockaddr*)&nra->m_addr,
            (socklen_t*)&WOS_SIZE_NRA);
	
		// nReceived is the number of bytes actually read from 
		// network interface
		if (nReceived > 0)
		{
			NetworkReceive ( m_rxbuffer, nReceived, nra);
		}
		else
		{
			delete nra;
			morePackets = false;	// stop receiving process
		}
    }
	return WOS_ERR_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////

/** The program's main function. The user can pass a port number at which
 * the OSC server starts listening.
 */
int main(int argc, char *argv[])
{
	int port = OSC_SERVER_PORT;
	if ( argc > 1 )
	{
		port = atoi( argv[1] );
	}
	
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
	std::cout << "WOscServer 1.00 (UNIX/POSIX build).\n" ;
#elif OS_IS_WIN32 == 1
	std::cout << "WOscServer 1.00 (Windows build).\n" ;
#endif

	// the one and only server object
	WOscServer server;
	
	std::cout << "Initializing Network Layer (port: " << port << ")\n" ;

	if ( server.NetworkInit( port ) != WOscServer::WOS_ERR_NO_ERROR )
	{
		std::cout << "Exit.\n" ;
		return -1;
	}
	
	std::cout << "Entering main loop and waiting for OSC packets...\n" ;

	// run osc server as long the "exit" flag isn't set.
	while ( ! server.Exit() )
	{
		server.CheckForPackets();

		// poll for new packets every millisecond
		// (this demo is non-blocking)
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
		usleep(1000);
#elif OS_IS_WIN32 == 1
		Sleep(1);
#endif
    }

	// stop network layer
	server.NetworkHalt();
	
	std::cout << "Exiting. Bye.\n" ;
	return 0;
}
