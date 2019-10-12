/*******************************************************************************
 *
 * File: Socket.h
 * 	Generic System Interface classes for working with sockets
 *
 * Written by:
 * 	The Robonauts
 * 	FRC Team 118
 * 	NASA, Johnson Space Center
 * 	Clear Creek Independent School District
 *
 *
 *  This code is based on PracticalSocket, http://cs.ecs.baylor.edu/~donahoo/practical/CSockets/practical/,
 *  I expect that is will change over time but will probably always look a lot like PracticalSocket
 *
 ******************************************************************************/
#pragma once

#ifdef WIN32
#pragma warning( disable : 4290 )	// C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning( disable : 4996 )	// 'strerror': This function or variable may be unsafe. Consider using strerror_s instead.
#endif

#include <stdint.h>
#include <string>

#include <gsi/Exception.h>

using namespace std;

namespace gsi
{

/**
*   Base class representing basic communication endpoint
*/
class Socket 
{
	public:
		/**
		*   Close and deallocate this socket
		*/
		~Socket();

		/**
		*   Get the local address
		*   @return local address of socket
		*   @exception SocketException thrown if fetch fails
		*/
		string getLocalAddress() throw(gsi::Exception);

		/**
		*   Get the local port
		*   @return local port of socket
		*   @exception SocketException thrown if fetch fails
		*/
		unsigned short getLocalPort() throw(gsi::Exception);

		/**
		*   Set the local port to the specified port and the local address
		*   to any interface
		*   @param localPort local port
		*   @exception SocketException thrown if setting local port fails
		*/
		void setLocalPort(unsigned short localPort) throw(gsi::Exception);

		/**
		*   Set the local port to the specified port and the local address
		*   to the specified address.  If you omit the port, a random port 
		*   will be selected.
		*   @param localAddress local address
		*   @param localPort local port
		*   @exception SocketException thrown if setting local port or address fails
		*/
		void setLocalAddressAndPort(const string &localAddress, 
			unsigned short localPort = 0) throw(gsi::Exception);

		/*
		 * Get the raw file descriptor, usage should be rare, but need it
		 * for doing a select() for now, would like to move select 
		 * logic to this file.
		 */
		int getDescriptor(void);

	protected:
		int m_socket_descriptor;
		Socket(int type, int protocol)  throw(gsi::Exception);
		Socket(int socket_descriptor);

	private:
		// Prevent the user from trying to use value semantics on this object
		Socket(const Socket &sock);
		void operator=(const Socket &sock);
};

/**
*   Socket which is able to connect, send, and receive
*/
class CommunicatingSocket : public Socket 
{
	public:
		/**
		*   Establish a socket connection with the given foreign
		*   address and port
		*   @param foreignAddress foreign address (IP address or name)
		*   @param foreignPort foreign port
		*   @exception SocketException thrown if unable to establish connection
		*/
		void connect(const string &foreignAddress, unsigned short foreignPort)
			throw(gsi::Exception);

		bool isConnected(void);

		/**
		*   Write the given buffer to this socket.  Call connect() before
		*   calling send()
		*   @param buffer buffer to be written
		*   @param bufferLen number of bytes from buffer to be written
		*   @exception SocketException thrown if unable to send data
		*/
        void send(const void *buffer, int bufferLen, int flags = 0) throw(gsi::Exception);

		/**
		*   Read into the given buffer up to bufferLen bytes data from this
		*   socket.  Call connect() before calling recv()
		*   @param buffer buffer to receive the data
		*   @param bufferLen maximum number of bytes to read into buffer
		*   @return number of bytes read, 0 for EOF, and -1 for error
		*   @exception SocketException thrown if unable to receive data
		*/
		int recv(void *buffer, int bufferLen) throw(gsi::Exception);

		/**
		*   Get the foreign address.  Call connect() before calling recv()
		*   @return foreign address
		*   @exception SocketException thrown if unable to fetch foreign address
		*/
		string getForeignAddress() throw(gsi::Exception);

		/**
		*   Get the foreign port.  Call connect() before calling recv()
		*   @return foreign port
		*   @exception SocketException thrown if unable to fetch foreign port
		*/
		unsigned short getForeignPort() throw(gsi::Exception);

	protected:
		CommunicatingSocket(int type, int protocol);
		CommunicatingSocket(int newConnSD);

		bool m_is_connected;
};

/**
*   TCP socket for communication with other TCP sockets
*/
class TCPSocket : public CommunicatingSocket 
{
	public:
		/**
		*   Construct a TCP socket with no connection
		*   @exception SocketException thrown if unable to create TCP socket
		*/
		TCPSocket();

		/**
		*   Construct a TCP socket with a connection to the given foreign address
		*   and port
		*   @param foreignAddress foreign address (IP address or name)
		*   @param foreignPort foreign port
		*   @exception SocketException thrown if unable to create TCP socket
		*/
		TCPSocket(const string &foreignAddress, unsigned short foreignPort)
			throw(gsi::Exception);

	private:
		// Access for TCPServerSocket::accept() connection creation
		friend class TCPServerSocket;
		TCPSocket(int newConnSD);
};

/**
*   TCP socket class for servers
*/
class TCPServerSocket : public Socket 
{
	public:
		/**
		*   Construct a TCP socket for use with a server, accepting connections
		*   on the specified port on any interface
		*   @param localPort local port of server socket, a value of zero will
		*                   give a system-assigned unused port
		*   @param queueLen maximum queue length for outstanding 
		*                   connection requests (default 5)
		*   @exception SocketException thrown if unable to create TCP server socket
		* 
		*/
		TCPServerSocket(unsigned short localPort, int queueLen = 5);

		/**
		*   Construct a TCP socket for use with a server, accepting connections
		*   on the specified port on the interface specified by the given address
		*   @param localAddress local interface (address) of server socket
		*   @param localPort local port of server socket
		*   @param queueLen maximum queue length for outstanding 
		*                   connection requests (default 5)
		*   @exception SocketException thrown if unable to create TCP server socket
		*/
		TCPServerSocket(const string &localAddress, unsigned short localPort,
			int queueLen = 5);

		/**
		*   Blocks until a new connection is established on this socket or error
		*   @return new connection socket
		*   @exception SocketException thrown if attempt to accept a new connection fails
		*/
		TCPSocket *accept() throw(gsi::Exception);

	private:
		void setListen(int queueLen) throw(gsi::Exception);
};

/**
*   UDP socket class
*/
class UDPSocket : public CommunicatingSocket 
{
	public:
		/**
		*   Construct a UDP socket
		*   @exception SocketException thrown if unable to create UDP socket
		*/
		UDPSocket(uint32_t timeoutmsA = 0);

		/**
		*   Construct a UDP socket with the given local port
		*   @param localPort local port
		*   @exception SocketException thrown if unable to create UDP socket
		*/
		UDPSocket(uint16_t localPort, uint32_t timeoutmsA = 0);

		/**
		*   Construct a UDP socket with the given local port and address
		*   @param localAddress local address
		*   @param localPort local port
		*   @exception SocketException thrown if unable to create UDP socket
		*/
		UDPSocket(const string &localAddress, uint16_t localPort, uint32_t timeoutmsA = 0);

		/**
		*   Unset foreign address and port
		*   @return true if disassociation is successful
		*   @exception SocketException thrown if unable to disconnect UDP socket
		*/
		void disconnect() throw(gsi::Exception);

		// 0 for blocking is enabled 1 for blocking disabled
//		int setBlocking(int iModeA);

		/**
		*   Send the given buffer as a UDP datagram to the
		*   specified address/port
		*   @param buffer buffer to be written
		*   @param bufferLen number of bytes to write
		*   @param foreignAddress address (IP address or name) to send to
		*   @param foreignPort port number to send to
		*   @return true if send is successful
		*   @exception SocketException thrown if unable to send datagram
		*/
		int32_t sendTo(const void *buffer,
					   uint32_t bufferLen,
					   const string &foreignAddress,
					   uint16_t foreignPort);

		/**
		*   Read up to bufferLen bytes data from this socket.  The given buffer
		*   is where the data will be placed
		*   @param buffer buffer to receive data
		*   @param bufferLen maximum number of bytes to receive
		*   @param sourceAddress address of datagram source
		*   @param sourcePort port of data source
		*   @return number of bytes received and -1 for error
		*   @exception SocketException thrown if unable to receive datagram
		*/
		int recvFrom(void *buffer, uint32_t bufferLen, string &sourceAddress, 
			uint16_t &sourcePort) throw(gsi::Exception);
		int recvFrom(void *buffer, uint32_t bufferLen, string &sourceAddress, 
			uint16_t &sourcePort, double timeout);

		/**
		*   Set the multicast TTL
		*   @param multicastTTL multicast TTL
		*   @exception SocketException thrown if unable to set TTL
		*/
		void setMulticastTTL(unsigned char multicastTTL) throw(gsi::Exception);

		/**
		*   Join the specified multicast group
		*   @param multicastGroup multicast group address to join
		*   @exception SocketException thrown if unable to join group
		*/
		void joinGroup(const string &multicastGroup) throw(gsi::Exception);

		/**
		*   Leave the specified multicast group
		*   @param multicastGroup multicast group address to leave
		*   @exception SocketException thrown if unable to leave group
		*/
		void leaveGroup(const string &multicastGroup) throw(gsi::Exception);

	private:
		void setBroadcast();
		void setTimeout(uint32_t msA);
		char sourceName[256];
};

} // end namespace gsi
