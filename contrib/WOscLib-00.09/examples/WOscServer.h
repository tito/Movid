

/** The network return address. Through multiple inheritance this object
 * inherits the functionality required by WOscLib to keep it alive until
 * all messages ob a bundle are processed (reference counting) and the
 * networking data structure which contains the actual ip/port information.
 *
 */
class TheNetReturnAddress:
	public WOscNetReturn 
{
public:
	struct sockaddr_in	m_addr;
};

///////////////////////////////////////////////////////////////////////////////

class WOscServer;

/** All methods used in WOscServer are derived from this class. In a real
 * project the methods should be implemented in a header file to inline them
 * and eliminate the c++ overhead.
 *
 * This class avoids some pointer-casting but wouldn't be necessary but makes
 * all methods used type-save.
 */
class WOscServerMethod:
	public WOscReceiverMethod
{
public:
	WOscServerMethod(
		WOscContainer* parent,
		WOscServer* receiverContext,
		const char* methodName,
		const char* methodDescription);
	virtual void Method(
		const WOscMessage *message,
		const WOscTimeTag& when,
		const TheNetReturnAddress* networkReturnAddress) = 0;
	virtual WOscServer* GetContext();
protected:
	virtual void Method(
		const WOscMessage *message,
		const WOscTimeTag& when,
		const WOscNetReturn* networkReturnAddress);
};

///////////////////////////////////////////////////////////////////////////////

/** A simple method which prints "hello world" on the console. Furthermore
 * All parameters send with a "/hello" message are printed on the console.
 *
 */
class TheOscHelloMethod:
	public WOscServerMethod
{
public:
	TheOscHelloMethod(
		WOscContainer* parent,
		WOscServer* receiverContext);
	virtual void Method(
		const WOscMessage *message,
		const WOscTimeTag& when,
		const TheNetReturnAddress* networkReturnAddress);
};

///////////////////////////////////////////////////////////////////////////////

/** This message sets an exit-request, what results that the main while
 * loop condition evaluates to false and the application exits.
 */
class TheOscExitMethod:
	public WOscServerMethod
{
public:
	TheOscExitMethod(
		WOscContainer* parent,
		WOscServer* receiverContext);
	virtual void Method(
		const WOscMessage *message,
		const WOscTimeTag& when,
		const TheNetReturnAddress* networkReturnAddress);
};

///////////////////////////////////////////////////////////////////////////////

/** A simple method which prints "hello world" on the console. Furthermore
 * All parameters send with a "/hello" message are printed on the console.
 *
 */
class TheOscEchoMethod:
	public WOscServerMethod
{
public:
	TheOscEchoMethod(
		WOscContainer* parent,
		WOscServer* receiverContext);
	virtual void Method(
		const WOscMessage *message,
		const WOscTimeTag& when,
		const TheNetReturnAddress* networkReturnAddress);
};

///////////////////////////////////////////////////////////////////////////////

/** This is the central class which implements a full featured OSC server
 * including the network-layer implementation.
 *
 */
class WOscServer:
	public WOscReceiver
{
public:
	WOscServer();
	virtual ~WOscServer();
	/** Errors which can occur in the network layer. */
	enum WOscServerErrors
	{
		WOS_ERR_NO_ERROR         =  0, /**< No error.                        */
		WOS_ERR_SOCKET_CREATE    = -1, /**< Error when creating the socket.  */
		WOS_ERR_SOCKET_BIND      = -2, /**< Error when binding the local
		                                * address and port to the socket.    */
		WOS_ERR_SOCKET_REUSE     = -3, /**< Error when setting the socket-
		                                * reuse option.                      */
		WOS_ERR_SOCKET_BROADCAST = -4, /**< Error when setting the socket-
		                                * broadcast option.                  */
		WOS_ERR_SOCKET_BLOCK     = -5, /**< Error when setting the socket to
		                                * the non-blocking mode.             */
		WOS_ERR_SOCKET_WSASTART  = -6, /**< Error when starting the Windoze
		                                * winsock subsystem (Windows builds
		                                * only).                             */
	};
	WOscServerErrors NetworkInit(int port);
	WOscServerErrors NetworkHalt();
	WOscServerErrors CheckForPackets();
	bool Exit() const ;
	void SetExit() ;

	virtual void NetworkSend(
		const char* data,
		int dataLen,
		const WOscNetReturn* networkReturnAddress);
		
protected:
	virtual void HandleOffendingPackets(
		const char* const data,
		int dataLen,
		WOscException* exception);
	virtual void HandleNonmatchedMessages(
		const WOscMessage* msg,
		const WOscNetReturn* networkReturnAddress);

private:
	/** The network address of the server when initialized.                  */
	TheNetReturnAddress		m_serverAddr;
	/** Databuffer for received data.                                        */
	char*					m_rxbuffer;
	/** The socket handler of this OSC server.                               */
	int						m_hSocket;
	/** Flag set when termination of the application or server is desired.   */
	bool					m_exit;
};




