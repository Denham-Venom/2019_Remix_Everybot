/*******************************************************************************
 *
 * File: Socket.cpp
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
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "gsi/Socket.h"

#ifdef WIN32
#include <Ws2tcpip.h>
#include <winsock.h>         // For socket(), connect(), send(), and recv()

typedef int socklen_t;
typedef char raw_type;       // Type used for raw data on this platform
#else
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
#include <string.h>
typedef void raw_type;       // Type used for raw data on this platform
#endif

#include <errno.h>             // For errno

using namespace std;

#ifdef WIN32
static bool initialized = false;
#endif

namespace gsi
{

/*******************************************************************************
 *
 * Fill the provided socket address structure based on the provided address
 * and port.
 *
 * @param	address	a string representation of the address or host name
 * @param	port	the port number that should be used
 * @param	addr	a reference to the structure that is to be filled
 *
 ******************************************************************************/
static void fillAddr(const string &address, unsigned short port, 
	sockaddr_in &addr) 
{
		memset(&addr, 0, sizeof(addr));  // Zero out address structure

		hostent *host;  // Resolve name
		if ((host = gethostbyname(address.c_str())) == NULL) 
		{
			throw gsi::Exception("Failed to resolve name (gethostbyname())", errno, __FILE__, __LINE__);
		}

		addr.sin_family = host->h_addrtype;
		addr.sin_addr.s_addr = *((unsigned long *)host->h_addr_list[0]);
		addr.sin_port = htons(port); 
}

// Socket Code

/*******************************************************************************
 *
 ******************************************************************************/
Socket::Socket(int type, int protocol) throw(gsi::Exception) 
{
	m_socket_descriptor = -1;

#ifdef WIN32
	if (!initialized) 
	{
		WSADATA wsaData;
		// Load WinSock DLL, Request WinSock v2.0
		int wsa_ret = WSAStartup( MAKEWORD(2, 0), &wsaData);
		if (wsa_ret != 0) 
		{  
			throw gsi::Exception("Unable to load WinSock DLL", wsa_ret, __FILE__, __LINE__);
		}
		initialized = true;
	}
#endif

	// Make a new socket
	m_socket_descriptor = socket(PF_INET, type, protocol);
	if (m_socket_descriptor < 0)
	{
		throw gsi::Exception("Unable to create socket", errno, __FILE__, __LINE__);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
Socket::Socket(int socket_descriptor) 
{
	m_socket_descriptor = socket_descriptor;
}

/*******************************************************************************
 *
 ******************************************************************************/
Socket::~Socket() 
{
	if (m_socket_descriptor >= 0)
	{
#ifdef WIN32
		::closesocket(m_socket_descriptor);
#else
		::close(m_socket_descriptor);
#endif
	}
	m_socket_descriptor = -1;
}

/*******************************************************************************
 *
 ******************************************************************************/
string Socket::getLocalAddress() throw(gsi::Exception) 
{
	if (m_socket_descriptor < 0)
	{
		throw gsi::Exception("Socket not initialized", -1, __FILE__, __LINE__);
	}

	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getsockname(m_socket_descriptor, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
	{
		throw gsi::Exception("Fetch of local address failed (getsockname())", errno, __FILE__, __LINE__);
	}
	return inet_ntoa(addr.sin_addr);
}

/*******************************************************************************
 *
 ******************************************************************************/
unsigned short Socket::getLocalPort() throw(gsi::Exception) 
{
	if (m_socket_descriptor < 0)
	{
		throw gsi::Exception("Socket not initialized", -1, __FILE__, __LINE__);
	}

	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getsockname(m_socket_descriptor, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
	{
		throw gsi::Exception("Fetch of local port failed (getsockname())", errno, __FILE__, __LINE__);
	}
	return ntohs(addr.sin_port);
}

/*******************************************************************************
 *
 ******************************************************************************/
void Socket::setLocalPort(unsigned short localPort) throw(gsi::Exception) 
{
	if (m_socket_descriptor < 0)
	{
		throw gsi::Exception("Socket not initialized", -1, __FILE__, __LINE__);
	}

	// Bind the socket to its port
	sockaddr_in localAddr;
	memset(&localAddr, 0, sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(localPort);

	if (bind(m_socket_descriptor, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) 
	{
		throw gsi::Exception("Set of local port failed (bind())", errno, __FILE__, __LINE__);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void Socket::setLocalAddressAndPort(const string &localAddress,
	unsigned short localPort) throw(gsi::Exception) 
{
	if (m_socket_descriptor < 0)
	{
		throw gsi::Exception("Socket not initialized", -1, __FILE__, __LINE__);
	}

	// Get the address of the requested host
	sockaddr_in localAddr;
	fillAddr(localAddress, localPort, localAddr);

	if (bind(m_socket_descriptor, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0) 
	{
		throw gsi::Exception("Set of local address and port failed (bind())", errno, __FILE__, __LINE__);
	}
}

int Socket::getDescriptor(void)
{
	return m_socket_descriptor;
}

// CommunicatingSocket Code

/*******************************************************************************
 *
 ******************************************************************************/
CommunicatingSocket::CommunicatingSocket(int type, int protocol)  
	: Socket(type, protocol) 
{
	m_is_connected = false;
}

/*******************************************************************************
 *
 ******************************************************************************/
CommunicatingSocket::CommunicatingSocket(int socket_descriptor) 
	: Socket(socket_descriptor) 
{
	m_is_connected = false;
}

/*******************************************************************************
 *
 ******************************************************************************/
void CommunicatingSocket::connect(const string &foreignAddress,
	unsigned short foreignPort) throw(gsi::Exception) 
{
	if (false == m_is_connected)
	{
		// Get the address of the requested host
		sockaddr_in destAddr;
		fillAddr(foreignAddress, foreignPort, destAddr);

		// Try to connect to the given port
		if (::connect(m_socket_descriptor, (sockaddr *) &destAddr, sizeof(destAddr)) < 0) 
		{
			throw gsi::Exception("Connect failed (connect())", errno, __FILE__, __LINE__);
		}

		m_is_connected = true;
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
bool CommunicatingSocket::isConnected(void)
{
	return ((m_socket_descriptor >= 0) & m_is_connected);
}

/*******************************************************************************
 *
 ******************************************************************************/
void CommunicatingSocket::send(const void *buffer, int bufferLen, int flags)
	throw(gsi::Exception) 
{
        if (::send(m_socket_descriptor, (raw_type *) buffer, bufferLen, flags) < 0)
        {
			throw gsi::Exception("Send failed (send())", errno, __FILE__, __LINE__);
		}
}

/*******************************************************************************
 *
 ******************************************************************************/
int CommunicatingSocket::recv(void *buffer, int bufferLen) 
	throw(gsi::Exception) 
{
		int rtn;
		if ((rtn = ::recv(m_socket_descriptor, (raw_type *) buffer, bufferLen, 0)) < 0) 
		{
			throw gsi::Exception("Received failed (recv())", errno, __FILE__, __LINE__);
		}

		return rtn;
}

/*******************************************************************************
 *
 ******************************************************************************/
string CommunicatingSocket::getForeignAddress() 
	throw(gsi::Exception) 
{
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);

		if (getpeername(m_socket_descriptor, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) 
		{
			throw gsi::Exception("Fetch of foreign address failed (getpeername())", errno, __FILE__, __LINE__);
		}
		return inet_ntoa(addr.sin_addr);
}

/*******************************************************************************
 *
 ******************************************************************************/
unsigned short CommunicatingSocket::getForeignPort() throw(gsi::Exception) 
{
	sockaddr_in addr;
	unsigned int addr_len = sizeof(addr);

	if (getpeername(m_socket_descriptor, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) 
	{
		throw gsi::Exception("Fetch of foreign port failed (getpeername())", errno, __FILE__, __LINE__);
	}
	return ntohs(addr.sin_port);
}


// TCPSocket Code

/*******************************************************************************
 *
 ******************************************************************************/
TCPSocket::TCPSocket()
	: CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP) 
{
}

/*******************************************************************************
 *
 ******************************************************************************/
TCPSocket::TCPSocket(int socket_descriptor) 
	: CommunicatingSocket(socket_descriptor) 
{
}

/*******************************************************************************
 *
 ******************************************************************************/
TCPSocket::TCPSocket(const string &foreignAddress, unsigned short foreignPort)
	throw(gsi::Exception)
	: CommunicatingSocket(SOCK_STREAM, IPPROTO_TCP)
{
	connect(foreignAddress, foreignPort);
}


// TCPServerSocket Code

/*******************************************************************************
 *
 ******************************************************************************/
TCPServerSocket::TCPServerSocket(unsigned short localPort, int queueLen) 
	: Socket(SOCK_STREAM, IPPROTO_TCP) 
{
		setLocalPort(localPort);
		setListen(queueLen);
}

/*******************************************************************************
 *
 ******************************************************************************/
TCPServerSocket::TCPServerSocket(const string &localAddress, 
	unsigned short localPort, int queueLen) 
	: Socket(SOCK_STREAM, IPPROTO_TCP)
{
		setLocalAddressAndPort(localAddress, localPort);
		setListen(queueLen);
}

/*******************************************************************************
 *
 ******************************************************************************/
TCPSocket *TCPServerSocket::accept() throw(gsi::Exception) 
{
	int newConnSD;
	if ((newConnSD = ::accept(m_socket_descriptor, NULL, 0)) < 0) 
	{
		throw Exception("Accept failed (accept())", true);
	}

	return new TCPSocket(newConnSD);
}

/*******************************************************************************
 *
 ******************************************************************************/
void TCPServerSocket::setListen(int queueLen) throw(gsi::Exception) 
{
	if (listen(m_socket_descriptor, queueLen) < 0) 
	{
		throw gsi::Exception("Set listening socket failed (listen())", -1, __FILE__, __LINE__);
	}
}

// UDPSocket Code

/*******************************************************************************
 *
 ******************************************************************************/
UDPSocket::UDPSocket(uint32_t timeoutmsA) 
	: CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP)
{
	setBroadcast();
	if(timeoutmsA > 0)
	{
		setTimeout(timeoutmsA);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
UDPSocket::UDPSocket(uint16_t localPort, uint32_t timeoutmsA)
	: CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP)
{
	setLocalPort(localPort);
	setBroadcast();
	if(timeoutmsA > 0)
	{
		setTimeout(timeoutmsA);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
UDPSocket::UDPSocket(const string &localAddress, uint16_t localPort, uint32_t timeoutmsA) 
	: CommunicatingSocket(SOCK_DGRAM, IPPROTO_UDP)
{
	setLocalAddressAndPort(localAddress, localPort);
	setBroadcast();
	if(timeoutmsA > 0)
	{
		setTimeout(timeoutmsA);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void UDPSocket::setBroadcast()
{
	// If this fails, we'll hear about it when we try to send.  This will allow 
	// system that cannot broadcast to continue if they don't plan to broadcast
	int broadcastPermission = 1;

	setsockopt(m_socket_descriptor, SOL_SOCKET, SO_BROADCAST, 
		(raw_type *) &broadcastPermission, sizeof(broadcastPermission));
}

/*******************************************************************************
 *
 ******************************************************************************/
void UDPSocket::setTimeout(uint32_t msA) 
{
#ifdef WIN32
	DWORD timeout = msA;
	setsockopt(m_socket_descriptor, SOL_SOCKET, SO_RCVTIMEO,(char *)&timeout, sizeof(timeout));
#else
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = msA*1000;
    setsockopt(m_socket_descriptor, SOL_SOCKET, SO_RCVTIMEO,(void *)&tv, sizeof(tv));
#endif
}

/*******************************************************************************
 *
 ******************************************************************************/
void UDPSocket::disconnect() throw(gsi::Exception) 
{
	sockaddr_in nullAddr;
	memset(&nullAddr, 0, sizeof(nullAddr));
	nullAddr.sin_family = AF_UNSPEC;

	// Try to disconnect
	if (::connect(m_socket_descriptor, (sockaddr *) &nullAddr, sizeof(nullAddr)) < 0) 
	{
#ifdef WIN32
		if (errno != WSAEAFNOSUPPORT) 
		{
#else
		if (errno != EAFNOSUPPORT)
		{
#endif
			throw gsi::Exception("Disconnect failed (connect())", -1, __FILE__, __LINE__);
		}
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
int32_t UDPSocket::sendTo(const void *buffer, uint32_t bufferLen,
	const string &foreignAddress, uint16_t foreignPort) 
{
	int32_t err = 0;
	sockaddr_in destAddr;
	fillAddr(foreignAddress, foreignPort, destAddr);

	// Write out the whole buffer as a single message.
	if (sendto(m_socket_descriptor, (raw_type *) buffer, bufferLen, 0,
        (sockaddr *) &destAddr, sizeof(destAddr)) != (int32_t)bufferLen)
    {
        err = -1;
	}
	return(err);
}

/*******************************************************************************
 *
 ******************************************************************************/
int UDPSocket::recvFrom(void *buffer, uint32_t bufferLen, string &sourceAddress,
	uint16_t &sourcePort, double timeoutSecA)
{
	sockaddr_in clntAddr;
	socklen_t addrLen = sizeof(clntAddr);
	fd_set fds;
	int n;
	struct timeval tv;

	// Set up the file descriptor set.
	FD_ZERO(&fds) ;
	FD_SET(m_socket_descriptor, &fds) ;

	// Set up the struct timeval for the timeout.
	tv.tv_sec = (uint32_t)timeoutSecA ;
	tv.tv_usec = (uint32_t)((timeoutSecA - tv.tv_sec) * 1000000);

	// Wait until timeout or data received.
	n = select ( m_socket_descriptor, &fds, NULL, NULL, &tv ) ;
	if( n == 0)
	{ 
		return 0;
	}
	else if(n < 0 )
	{
		return -1001;
	}

	n = recvfrom(m_socket_descriptor,(raw_type*)buffer,bufferLen,0,(sockaddr*)&clntAddr,(socklen_t*) &addrLen);
	if (n <= 0) 
	{
		return n;
	}

	sourceAddress = inet_ntoa(clntAddr.sin_addr);
	sourcePort = ntohs(clntAddr.sin_port);

	return(n);
}

/*******************************************************************************
 *
 ******************************************************************************/
int UDPSocket::recvFrom(void *buffer, uint32_t bufferLen, string &sourceAddress,
	uint16_t &sourcePort) throw(gsi::Exception) 
{
	sockaddr_in clntAddr;
	socklen_t addrLen = sizeof(clntAddr);
	int err = 0;
	int rtn;
	rtn = recvfrom(m_socket_descriptor, (raw_type*)buffer, bufferLen, 0, (sockaddr*)&clntAddr, (socklen_t*)&addrLen);
	if (rtn < 0)
	{
		err = errno;
		switch(err)
		{
			case 0:
			case EAGAIN:
			case ETIMEDOUT:
				return(-1);
			default:

				fprintf(stderr, "Socket Error : %s\n", strerror(err));
				break;
		}
	}
	sourceAddress = inet_ntoa(clntAddr.sin_addr);
	sourcePort = ntohs(clntAddr.sin_port);

	return rtn;
}

/*******************************************************************************
 *
 ******************************************************************************/
void UDPSocket::setMulticastTTL(unsigned char multicastTTL) throw(gsi::Exception) 
{
	if (setsockopt(m_socket_descriptor, IPPROTO_IP, IP_MULTICAST_TTL, 
		(raw_type *) &multicastTTL, sizeof(multicastTTL)) < 0) 
	{
			throw gsi::Exception("Multicast TTL set failed (setsockopt())", -1, __FILE__, __LINE__);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void UDPSocket::joinGroup(const string &multicastGroup) throw(gsi::Exception) 
{
	struct ip_mreq multicastRequest;

	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(m_socket_descriptor, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
		(raw_type *) &multicastRequest, 
		sizeof(multicastRequest)) < 0) 
	{
			throw gsi::Exception("Multicast group join failed (setsockopt())", -1, __FILE__, __LINE__);
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void UDPSocket::leaveGroup(const string &multicastGroup) throw(gsi::Exception) 
{
	struct ip_mreq multicastRequest;

	multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
	multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(m_socket_descriptor, IPPROTO_IP, IP_DROP_MEMBERSHIP, 
		(raw_type *) &multicastRequest, 
		sizeof(multicastRequest)) < 0) 
	{
			throw gsi::Exception("Multicast group leave failed (setsockopt())", -1, __FILE__, __LINE__);
	}
}

} // end namespace gsi
