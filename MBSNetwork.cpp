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

#include "MBSNetwork.h"
#include <string.h>
/*
    -------------------------------
    winsock2.h structures reference
    -------------------------------

    struct sockaddr_in
    {
        short int sin_family;           // always AF_INET
        unsigned short int sin_port;    // porta
        struct in_addr sin_addr;        // IP
        unsigned char sin_zero[8];      // Padding
    };

    struct in_addr
    {
        unsigned long s_addr;           // IP (4 bytes)
    };
*/

namespace {
#ifdef _WIN32
	static bool inicializado;
	void fimRede()
	{
		WSACleanup();
	}
	void iniciaRede()
	{
		WSADATA wsad;
		if(WSAStartup(MAKEWORD(2,2),&wsad)==0)
		{
			inicializado=true;
			atexit(&fimRede);
		}
	}
	static int sin_size=sizeof(sockaddr);
#else
	static socklen_t sin_size=sizeof(sockaddr);
#endif
};

using namespace std;

//------------------------------------------------------------------------------
//      MBSSocket
//------------------------------------------------------------------------------

MBSSocket::MBSSocket(SOCKET winfd, sockaddr_in *sadr)
{
#ifdef _WIN32
	if(!inicializado)
		iniciaRede();
#endif
	fd=winfd;
	if(sadr)
		memcpy(&dest,sadr,sin_size);
}

int MBSSocket::conectar(string ip, unsigned short port)
//  On return:
//  	 0: ok till now, connecting or connected
// 		-1: generic socket error
// 		-2: dns "error", host not found
{
	closeSocket();

	//solve addres using dns if necessary
	struct hostent *he;
    if((he=gethostbyname(ip.c_str()))==NULL)
        return -2;
    dest.sin_addr=*((in_addr*)he->h_addr);

    dest.sin_port=htons(port);
    dest.sin_family=AF_INET;
    memset(&(dest.sin_zero),0,8);

    if(INVALID_SOCKET==createSocket())
    	return -1;

	if(connect(fd,(sockaddr*)&dest,sizeof(sockaddr))==SOCKET_ERROR)
	{
		//TODO: trace error
		closeSocket();
		return -1;
	}
	return 0;
}

unsigned long MBSSocket::dns(string end)
{
	//TODO: arrumar dns()
	//  0 - host invalido
    struct hostent *he;
    if((he=gethostbyname(end.c_str()))==NULL)
        return 0;

//	cout << endl << "endereco:"<<end << endl;
//    cout << (int)*(he->h_addr);

    return *(he->h_addr);
}

SOCKET MBSSocket::closeSocket()
{
	if(valid())
#ifdef _WIN32
		closesocket(fd);
#else
		close(fd);
#endif
    fd=INVALID_SOCKET;
    return fd;
}

//------------------------------------------------------------------------------
//      MBSSocketServer
//------------------------------------------------------------------------------

int MBSSocketServer::ouvir(unsigned short port, int backlog)
//  On return:
//  	 0: ok, listenning
// 		-1: generic socket error
// 		-2: bind error, port may be already in use
{
	closeSocket();

    if(INVALID_SOCKET==createSocket())
    	return -1;
/*
	BOOL yes=TRUE;
	if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(BOOL))==SOCKET_ERROR)
	{
		closeSocket();
		return -1;
	}
*/
	dest.sin_family=AF_INET;
	dest.sin_port=htons(port);
	dest.sin_addr.s_addr=INADDR_ANY;
	memset(&(dest.sin_zero),0,8);

	if(bind(fd,(sockaddr*)&dest,sizeof(sockaddr))==SOCKET_ERROR)
	{
		closeSocket();
		return -2;
	}
	if(listen(fd,backlog)==-1)
	{
		closeSocket();
		return -1;
	}
    return 0;
}

MBSSocket* MBSSocketServer::aceitar()
{
	sockaddr_in adn;
    SOCKET fdn=accept(fd,(sockaddr*)&adn,&sin_size);
    if(fdn==INVALID_SOCKET)
        return NULL;
    return new MBSSocket(fdn,&adn);
}

void MBSSocketServer::refuse()
{
    sockaddr adn;
#ifdef _WIN32
	closesocket(accept(fd,&adn,&sin_size));
#else
	close(accept(fd,&adn,&sin_size));
#endif
}
