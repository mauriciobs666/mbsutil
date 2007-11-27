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
	static int sin_size=sizeof(sockaddr);
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
#endif
};

using namespace std;

//------------------------------------------------------------------------------
//      MBSSocket
//------------------------------------------------------------------------------

MBSSocket::MBSSocket(int winfd, sockaddr_in *sadr)
{
	if(!inicializado)
		iniciaRede();
	fd=winfd;
	if(sadr)
		memcpy(&dest,sadr,sin_size);
}

int MBSSocket::conectar(string ip, unsigned short port)
//  0: tudo ok, conectado
// -1: erro generico de conexao
// -2: erro de dns, host naum encontrado
{
	closeSocket();

	struct hostent *he;
    if((he=gethostbyname(ip.c_str()))==NULL)
        return -2;
    dest.sin_addr=*((in_addr*)he->h_addr);

    dest.sin_port=htons(port);
    dest.sin_family=AF_INET;
    memset(&(dest.sin_zero),0,8);

    if((int)INVALID_SOCKET==createSocket())
    	return -1;

	if(connect(fd,(sockaddr*)&dest,sizeof(sockaddr))==-1)
        if(WSAGetLastError()!=WSAEWOULDBLOCK)
        {
        	closeSocket();
            return -1;
        }

    return 0;
}

int MBSSocket::enviar(char *data, int len)
{
	int rc=send(fd,data,len,0);
	if(rc<=0)
		closeSocket();
    return rc;
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

int MBSSocket::closeSocket()
{
	if(valid())
		closesocket(fd);
    fd=INVALID_SOCKET;
    return fd;
}

//------------------------------------------------------------------------------
//      MBSSocketServer
//------------------------------------------------------------------------------

int MBSSocketServer::ouvir(unsigned short port, int backlog)
//	-1 : erro generico de conexao
//	-2 : erro de bind, porta em uso
{
	closeSocket();

    if((int)INVALID_SOCKET==createSocket())
    	return -1;

	dest.sin_family=AF_INET;
	dest.sin_port=htons(port);
	dest.sin_addr.s_addr=INADDR_ANY;
	memset(&(dest.sin_zero),0,8);

	if(bind(fd,(sockaddr*)&dest,sizeof(sockaddr))==-1)
	{
		closeSocket();
		return -2;
	}
	if(listen(fd,backlog)==-1)
	{
		closeSocket();
		return -2;
	}
    return 0;
}

MBSSocket* MBSSocketServer::aceitar()
{
	sockaddr_in adn;
    int fdn=accept(fd,(sockaddr*)&adn,&sin_size);
    if(fdn==(int)INVALID_SOCKET)
        return NULL;
    return new MBSSocket(fdn,&adn);
}

void MBSSocketServer::refuse()
{
    sockaddr adn;
    closesocket(accept(fd,&adn,&sin_size));
}
