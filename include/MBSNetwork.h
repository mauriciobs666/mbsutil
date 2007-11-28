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
	SOCKET fd;

    MBSSocket(SOCKET winfd=INVALID_SOCKET, sockaddr_in *sadr=NULL);
    virtual ~MBSSocket()
		{ closeSocket(); }
	bool valid()
		{ return (fd!=INVALID_SOCKET); }
    int conectar(std::string ip, unsigned short port);
    void disconnect()
		{ closeSocket(); }
    int enviar(char *data, int len)
		{ return send(fd,data,len,0); }
    int receive(char *data, int max)
		{ return recv(fd,data,max,0); }

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
	sockaddr_in dest;
    SOCKET createSocket()
		{ return fd=socket(PF_INET,SOCK_STREAM,0); }
	SOCKET closeSocket();
};

class MBSSocketServer : public MBSSocket
{
public:
	MBSSocketServer();
    int ouvir(unsigned short port, int backlog=10);
    MBSSocket* aceitar();
    void refuse();
    void disconnect()
		{ closeSocket(); }
};

class MBSSocketSelector
{
public:
	fd_set set_read;
	fd_set set_write;
	fd_set set_exception;
	timeval timeout;

	MBSSocketSelector();

	void add(SOCKET fd)
		{
			FD_SET(fd,&set_read);
			FD_SET(fd,&set_write);
			FD_SET(fd,&set_exception);
			if(fd>max_fd) max_fd=fd;
		}
	void remove(SOCKET fd)
		{
			FD_CLR(fd,&set_read);
			FD_CLR(fd,&set_write);
			FD_CLR(fd,&set_exception);
		}
	void clear()
		{
			FD_ZERO(&set_read);
			FD_ZERO(&set_write);
			FD_ZERO(&set_exception);
		}
	int Select()
		{ return select(max_fd+1,&set_read,&set_write,&set_exception,&timeout); }
	bool isRead(SOCKET fd)
		{ return FD_ISSET(fd,&set_read); }
	bool isWrite(SOCKET fd)
		{ return FD_ISSET(fd,&set_write); }
	bool isException(SOCKET fd)
		{ return FD_ISSET(fd,&set_exception); }
private:
	SOCKET max_fd;
};

#endif // MBSNETWORK_H_INCLUDED
