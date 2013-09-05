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
#else
	#include <unistd.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <sys/wait.h>
	#include <signal.h>
	#include <netdb.h>
	#define SOCKET int
	#define INVALID_SOCKET -1
	#define SOCKET_ERROR -1
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
    int enviar(char *data, size_t len)
		{ return send(fd,data,len,0); }
    int receive(char *data, size_t max)
		{ return recv(fd,data,max,0); }

	std::string remoteIP()
		{ return toString(dest.sin_addr.s_addr); }
	unsigned short remotePort()
		{ return ntohs(dest.sin_port); }
    sockaddr_in* getInfo()
		{ return &dest; }
    unsigned long dns(std::string end);
	std::string toString(in_addr_t ip)
//		{ return std::string(inet_ntoa(*((in_addr*)&ip))); }
		{
		    in_addr a;
		    a.s_addr = ip;
		    return std::string(inet_ntoa(a));
        }
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
	fd_set master_set;

	fd_set result_set_read;
	fd_set result_set_write;
	fd_set result_set_exception;

	timeval timeout;

	MBSSocketSelector()
		{
			clear();

			// timeout = 0 means "no blocking"
			timeout.tv_sec=0;
			timeout.tv_usec=0;
		}
	void add(SOCKET fd)
		{
			if(fd!=INVALID_SOCKET)
			{
				FD_SET(fd,&master_set);
				if(fd>max_fd) max_fd=fd;
			}
		}
	void remove(SOCKET fd)
		{
			if(fd!=INVALID_SOCKET)
				FD_CLR(fd,&master_set);
		}
	void clear()
		{
			FD_ZERO(&master_set);
			max_fd=0;
		}
	int Select()
		{
			result_set_read=master_set;
			result_set_write=master_set;
			result_set_exception=master_set;
			return select(max_fd+1,&result_set_read,&result_set_write,&result_set_exception,&timeout);
		}
	// Linux warning: the next 3 functions dump core if called with -1 (LOL)
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

/*
	Stripped down and optimized version option MBSSocketSelector
*/
//TODO: untested
class MBSClientSocketSelector
{
public:
	fd_set master_set;

	fd_set result_set_read;
	fd_set result_set_write;
	fd_set result_set_exception;

	timeval timeout;

	MBSClientSocketSelector()
		{
			// timeout = 0 means "no blocking"
			timeout.tv_sec=0;
			timeout.tv_usec=0;
		}
	int Select()
		{
			result_set_read=master_set;
			result_set_write=master_set;
			result_set_exception=master_set;
			return select(1,&result_set_read,&result_set_write,&result_set_exception,&timeout);
		}
	void set(SOCKET s)
	{
	    fd=s;
        FD_ZERO(&master_set);
        FD_SET(fd,&master_set);
    }
	// Linux warning: the next 3 functions dump core if called with -1 (LOL)
	bool isRead()		{ return FD_ISSET(fd,&result_set_read); }
	bool isWrite()		{ return FD_ISSET(fd,&result_set_write); }
	bool isException()	{ return FD_ISSET(fd,&result_set_exception); }
private:
	SOCKET fd;
};

#endif // MBSNETWORK_H_INCLUDED
