/*
	MBS-Util - General purpose C++ library
	Copyright (C) 2007 - Mauricio Bieze Stefani

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef MBSNETWORK_H_INCLUDED
#define MBSNETWORK_H_INCLUDED

#include <cstdlib>
#include <string>

#ifdef _WIN32
	#include <winsock2.h>
#endif

class MBSSocket
{
public:
    MBSSocket(int winfd=INVALID_SOCKET, sockaddr_in *sadr=NULL);
    virtual ~MBSSocket()
		{ closeSocket(); }
	bool valid()
		{ return (fd!=(int)INVALID_SOCKET); }
    int conectar(std::string ip, unsigned short port);
    void disconnect()
		{ closeSocket(); }
    int enviar(char *data, int len);
    int receive(char *data, int max);

	std::string remoteIP()
		{ return toString(dest.sin_addr.s_addr); }
	unsigned short remotePort()
		{ return ntohs(dest.sin_port); }
    sockaddr_in* getInfo()
		{ return &dest; }
    unsigned long dns(std::string end);
	std::string toString(unsigned long ip)
		{ return std::string(inet_ntoa(*((in_addr*)&ip))); }
protected:
    //win32 socket
    int fd;
	sockaddr_in dest;
    int createSocket()
		{ return fd=socket(PF_INET,SOCK_STREAM,0); }
    int closeSocket();
};

class MBSSocketServer : protected MBSSocket
{
public:
    int ouvir(unsigned short port, int backlog=10);
    MBSSocket* aceitar();
    void refuse();
    void disconnect()
		{ closeSocket(); }
};

class MBSSocketSelector
{
public:
	int add(int fd)
		{}
	int remove(int fd)
		{}
	void clear()
		{}
};

#endif // MBSNETWORK_H_INCLUDED
