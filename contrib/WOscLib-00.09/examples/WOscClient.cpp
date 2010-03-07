#include "config.h"
///////////////////////////////////////////////////////////////////////////////
// OS dependent includes
///////////////////////////////////////////////////////////////////////////////
   
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
#	include <unistd.h>		//	usleep
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

#include "WOscClient.h"

///////////////////////////////////////////////////////////////////////////////
// defines
///////////////////////////////////////////////////////////////////////////////
   
#define OSC_SERVER_PORT			10000
#define PROTOCOL_UDP			17
#define WOS_MAX_RX_UDP_PACKET	2048

const char* WOC_HELP_STR = 
	"\n"
	" WOscClient 1.00 help\n"
	"\n"
	" Invoking \"woscclient\":\n"
	" woscclient [local port] [remote port] [remote IP]\n"
	"\n"
	" Commands controlling \"woscclient\":\n"
	"\n"
	" exit        terminate \"woscclient\".\n"
	" slp [port]  set local UDP port to [port]\n"
	" srp [port]  set remote server UDP destination port to [port].\n"
	" srip [ip]   set remote server UDP destination IP to [ip].\n"
	"             whereas [ip] is an IP string in dotted decimal notation\n"
	"             (127.0.0.1).\n"
	"\n"
	" /[msg] [params]\n"
	"             send an OSC message \"/msg\" to remote OSC server with\n"
	"             parameters [params]. [params] can be a space separated\n"
	"             list containing integers ( 666 ), floats ( 3.14 ) and\n"
	"             strings ( hello_world ).\n"
	"\n";

#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
socklen_t WOS_SIZE_NRA = sizeof(sockaddr_in);
#else
int WOS_SIZE_NRA = sizeof(sockaddr_in);
#endif

///////////////////////////////////////////////////////////////////////////////
// WOscClientMethod
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Initializes WOscReceiverMethod base class. */
WOscClientMethod::WOscClientMethod(
	WOscContainer* parent,
	WOscClient* receiverContext,
	const char* methodName,
	const char* methodDescription)
:WOscReceiverMethod(
	parent,
	(WOscReceiver*)receiverContext,
	methodName,
	methodDescription)
{}

/** Type-save access to receiver object, which is owning this object.        */
WOscClient* WOscClientMethod::GetContext()
{
	return (WOscClient*)WOscReceiverMethod::GetContext();
}

/** Type-save method interface. All methods inherited from this class get
 * the correct TheNetReturnAddress pointers without caring about the
 * system below.
 */
void WOscClientMethod::Method(
	const WOscMessage *message,
	const WOscTimeTag& when,
	const WOscNetReturn* networkReturnAddress)
{
	Method(message, when, (TheNetReturnAddress*)networkReturnAddress);
}

///////////////////////////////////////////////////////////////////////////////
// TheOscEchoMethod
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Sets the method name and info in the base class.            */
TheOscEchoMethod::TheOscEchoMethod(
	WOscContainer* parent,
	WOscClient* receiverContext)
:WOscClientMethod(
	parent,
	receiverContext,
	"echo",
	"")
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
	std::cout << "Echo received from:\n" 
		<< "Port: " << ntohs(networkReturnAddress->m_addr.sin_port) << "\n"
		<< "IP:   " << inet_ntoa( networkReturnAddress->m_addr.sin_addr ) << "\n";
}

///////////////////////////////////////////////////////////////////////////////
// WOscClient
///////////////////////////////////////////////////////////////////////////////

/** Constructor. Sets up the OSC address space, allocates an UDP receive buffer
 * and initializes the exit flag to "false".
 */
WOscClient::WOscClient()
{
	// setup OSC address space //

	// containers
	WOscContainerInfo rootInfo("root");
	WOscContainer* rootContainer =
		new WOscContainer( &rootInfo );
	
	// "root" methods
	new TheOscEchoMethod( rootContainer, this );
	
	SetAddressSpace(rootContainer);

	// allocate RX buffer
    m_rxbuffer = new char[WOS_MAX_RX_UDP_PACKET];

	m_exit = false;
}

/** Destructor. Removes the address space and deallocates the UDP receive
 * buffer.
 */
WOscClient::~WOscClient()
{
	// remove address space //
	WOscContainer* as = GetAddressSpace();
	if ( as ) as->RemoveAll();
}

/** Checks the exit flag.
 * \returns
 * True if application/serve exit is requested.
 */
bool WOscClient::Exit() const
{
	return m_exit;
}

/** Sets the exit flag, which is used to check for server termination.       */
void WOscClient::SetExit()
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
void WOscClient::NetworkSend(
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
void WOscClient::HandleOffendingPackets(
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
void WOscClient::HandleNonmatchedMessages(
	const WOscMessage* msg,
	const WOscNetReturn* networkReturnAddress)
{
	int nStr = msg->GetNumStrings();
	int nInt = msg->GetNumInts();
	int nFlt = msg->GetNumFloats();

	if ( !nStr && !nInt && !nFlt )
		std::cout << "OSC message \"" << msg->GetOscAddress().GetBuffer()
			<< "\" received.\n";
	else
		std::cout << "OSC message \"" << msg->GetOscAddress().GetBuffer()
			<< "\" contains:\n";

	for (int i = 0; i < nStr; i++ )
		std::cout << "  str["<<i<<"]\t" << 
			msg->GetString(i).GetBuffer() << "\n" ;
	for (int i = 0; i < nInt; i++ )
		std::cout << "  int["<<i<<"]\t" << 
			msg->GetInt(i) << "\n" ;
	for (int i = 0; i < nFlt; i++ )
		std::cout << "  flt["<<i<<"]\t" << 
			msg->GetFloat(i) << "\n" ;
}

/** Initializes the underlying UDP network layer.
 * \param port
 * Port at which WOscClient should listen for OSC data.
 * \returns
 * Error code on error, WOS_ERR_NO_ERROR on success.
 */
WOscClient::WOscClientErrors WOscClient::NetworkInit(int port)
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
WOscClient::WOscClientErrors WOscClient::NetworkHalt()
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
WOscClient::WOscClientErrors WOscClient::CheckForPackets()
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

/** The program's main function of WOscClient.
 * Arguments:
 *  - 1. Local port at which the client listens.
 *  - 2. Remote port at which the remote server listens.
 *  - 3. Remote IP at which the remote server listens.
 *
 * When started, it accepts commandline input:
 *
 *  - "/message [params]" whereas "message" can be any OSC message as "/hello"
 *    and [params] can be some parameters (float, integer and string)
 *  - "exit" to exit WOscClient.
 *  - "slp port" to set local port to "port".
 */
int main(int argc, char *argv[])
{
	WOscClient client;
	int localport = OSC_SERVER_PORT + 1;
	int remoteport = OSC_SERVER_PORT;
	char remoteIP[32];
	
	strcpy(remoteIP, "127.0.0.1");

	// process arguments
	if ( argc > 1 )
		localport = atoi( argv[1] );
	if ( argc > 2 )
		remoteport = atoi( argv[2] );
	if ( argc > 3 )
		strcpy(remoteIP, argv[3]);
	
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
	std::cout << "WOscClient 1.00 (UNIX/POSIX build).\n" ;
#elif OS_IS_WIN32 == 1
	std::cout << "WOscClient 1.00 (Windows build).\n" ;
#endif

	std::cout << "Setting up network layer:\n" <<
		"  local port:  " << localport << "\n" <<
		"  remote port: " << remoteport << "\n" <<
		"  remote IP:   " << remoteIP << "\n";

	if ( client.NetworkInit( localport ) != WOscClient::WOS_ERR_NO_ERROR )
	{
		std::cout << "Exit.\n" ;
		return -1;
	}
	
	std::cout << "Please enter some commands...\n" ;
	
	// run osc server as long the "exit" flag isn't set.
	while ( ! client.Exit() )
	{
		char input[1024];
		std::cin.getline(input,1024,'\n');
		
		// checking for commands controlling WOscClient
		if ( input[0] != '/' )
		{
			// exit command
			if ( strcmp("exit",input) == 0 )
			{
				break;
			}				
			// help command
			else if ( strcmp("help",input) == 0 )
			{
				std::cout << WOC_HELP_STR ;
			}
			// set local port (slp)
			else if  ( strncmp("slp",input,3) == 0 )
			{
				localport = atoi( &input[4] );
				std::cout << "  Trying to set local port to " << localport << ".\n" ;
				client.NetworkHalt();
				if ( client.NetworkInit( localport ) != WOscClient::WOS_ERR_NO_ERROR )
				{
					std::cout << "Exit.\n" ;
					return -1;
				}
				std::cout << "  Port successfully changed and network restarted.\n" ;
			}
			// set remote port (srp)
			else if  ( strncmp("srp",input,3) == 0 )
			{
				remoteport = atoi( &input[4] );
				std::cout << "  Remote port set to " << remoteport << ".\n" ;
			}
			// set remote IP (srip)
			else if  ( strncmp("srip",input,4) == 0 )
			{
				strcpy(remoteIP, &input[5]);
				std::cout << "  Remote IP set to " << remoteIP << ".\n" ;
			}
		}
		// OSC message commands
		else
		{
			try
			{
				// parse parameters
				char* params[100];
				int nParams = 0;
				char* next = strchr ( input, ' ' );
				while ( next != 0 || nParams >= 100 )
				{
					// add parameter to list 
					params[nParams] = next + 1;
					// and zero-terminate previous
					*next = 0;
					// try to find new parameter
					next = strchr ( params[nParams++], ' ' );
				}
				
				// assemble message
				WOscMessage msg(input);
				// set destination address
				TheNetReturnAddress ra;
				ra.m_addr.sin_family         = AF_INET;
				ra.m_addr.sin_addr.s_addr    = inet_addr(remoteIP);
				ra.m_addr.sin_port           = htons(remoteport);
				
				// add parameters
				for ( int i = 0; i < nParams; i++ )
				{
					int parInt = atoi(params[i]);
					float parFlt = atof(params[i]);
					// check if float
					if ( strchr ( params[i], '.' ) )
					{
						msg.Add( parFlt );
					}else if ( parInt != 0 )
					{
						msg.Add( parInt );
					}else
					{
						msg.Add( params[i] );
					}
				}
				client.NetworkSend(msg.GetBuffer(), msg.GetBufferLen(), &ra);
			}
			catch( WOscException* e )
			{
				std::cout<<"Exception: " << e->GetDescription() << "\n";
				e->Destroy();
			}
			catch( ... )
			{
				std::cout<<"Exception in OSC message construction.\n";
			}
		}
#if OS_IS_LINUX == 1 || OS_IS_MACOSX == 1 || OS_IS_CYGWIN == 1
		usleep(100000);
#elif OS_IS_WIN32 == 1
		Sleep(100);
#endif
		client.CheckForPackets();
    }

	// stop network layer
	client.NetworkHalt();
	
	std::cout << "Exiting. Bye.\n" ;
	return 0;
}
