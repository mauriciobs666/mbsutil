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
    int ouvir(unsigned short port, int backlog=10);
    MBSSocket* aceitar();
    void refuse();
};

class MBSSocketSelector
{
public:
	fd_set master_set_read;
	fd_set master_set_write;
	fd_set master_set_exception;

	fd_set result_set_read;
	fd_set result_set_write;
	fd_set result_set_exception;

	timeval timeout;

	MBSSocketSelector()
		{
			clear();
			timeout.tv_sec=0;
			timeout.tv_usec=0;
		}
	void add(SOCKET fd)
		{
			FD_SET(fd,&master_set_read);
			FD_SET(fd,&master_set_write);
			FD_SET(fd,&master_set_exception);
			if(fd>max_fd) max_fd=fd;
		}
	void remove(SOCKET fd)
		{
			FD_CLR(fd,&master_set_read);
			FD_CLR(fd,&master_set_write);
			FD_CLR(fd,&master_set_exception);
		}
	void clear()
		{
			FD_ZERO(&master_set_read);
			FD_ZERO(&master_set_write);
			FD_ZERO(&master_set_exception);
			max_fd=0;
		}
	int Select()
		{
			result_set_read=master_set_read;
			result_set_write=master_set_write;
			result_set_exception=master_set_exception;
			return select(max_fd+1,&result_set_read,&result_set_write,&result_set_exception,&timeout);
		}
	bool isRead(SOCKET fd)
		{
			return FD_ISSET(fd,&result_set_read);
		}
	bool isWrite(SOCKET fd)
		{
			return FD_ISSET(fd,&result_set_write);
		}
	bool isException(SOCKET fd)
		{
			return FD_ISSET(fd,&result_set_exception);
		}
private:
	SOCKET max_fd;
};

#endif // MBSNETWORK_H_INCLUDED
