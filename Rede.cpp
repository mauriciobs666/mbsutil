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

#ifdef _WIN32

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include "Rede.h"

namespace {
	static int sin_size=sizeof(sockaddr);
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
};

using namespace std;

//------------------------------------------------------------------------------
//      Conexao
//------------------------------------------------------------------------------

Conexao::Conexao()
{
	if(!inicializado)
		iniciaRede();

	fd=INVALID_SOCKET;
    pai=NULL;
    id=0;
	callback=NULL;
    threadEventosHnd=NULL;
}

Conexao::Conexao(int soquete)
{
    pai=NULL;
    id=0;
	callback=NULL;
    threadEventosHnd=NULL;
	fd=soquete;
    getpeername(fd,(sockaddr*)&dest,&sin_size);
}

Conexao::~Conexao()
{
	desconectar();
}

bool Conexao::ativa()
{
    if(fd==INVALID_SOCKET)
    {
        fimTE();
        return false;
    }
    return true;
}

int Conexao::conectar(const char *end, const unsigned short porta)
//  0: tudo ok, conectado
// -1: erro generico de conexao
// -2: erro de dns, host naum encontrado
{
    struct hostent *he;

	if(ativa())
		return -1;
    if((he=gethostbyname(end))==NULL)
        return -2;
    return conectar(ntohl((*((in_addr*)he->h_addr)).S_un.S_addr),porta);
}

int Conexao::conectar(unsigned long end, unsigned short porta)
{
	dest.sin_addr.S_un.S_addr=htonl(end);
    dest.sin_port=htons(porta);
    dest.sin_family=AF_INET;
    memset(&(dest.sin_zero),0,8);
    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==INVALID_SOCKET)
    	return -1;
    iniTE();
	if(connect(fd,(sockaddr*)&dest,sizeof(sockaddr))==-1)
        if(WSAGetLastError()!=WSAEWOULDBLOCK)
            return -1;
    return 0;
}

void Conexao::desconectar()
{
    fimTE();
    if(ativa())
	{
    	closesocket(fd);
    	fd=INVALID_SOCKET;
    }
}

int Conexao::ouvir(unsigned short porta, int backlog)
//	-1 : erro generico de conexao
//	-2 : erro de bind, porta em uso
{
	if(ativa())
		return -1;

    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==INVALID_SOCKET)
    	return -1;

	dest.sin_family=AF_INET;
	dest.sin_port=htons(porta);
	dest.sin_addr.s_addr=INADDR_ANY;
	memset(&(dest.sin_zero),0,8);

	if(bind(fd,(sockaddr*)&dest,sizeof(sockaddr))==-1)
	{
		closesocket(fd);
		fd=INVALID_SOCKET;
		return -2;
	}
	if(listen(fd,backlog)==-1)
	{
		closesocket(fd);
		fd=INVALID_SOCKET;
		return -2;
	}
    iniTE();
    return 0;
}

Conexao* Conexao::aceitar()
{
	Conexao *nova;
    sockaddr_in adn;
    unsigned fdn=accept(fd,(sockaddr*)&adn,&sin_size);
    if(fdn==INVALID_SOCKET)
        return NULL;
    nova=new Conexao(fdn);
    nova->registraCallbackFilho(callbackfilho);
    return nova;
}

void Conexao::recusar()
{
    sockaddr_in adn;
    closesocket(accept(fd,(sockaddr*)&adn,&sin_size));
}

int Conexao::enviar(char *dados, int len)
{
    return send(fd,dados,len,0);
}

int Conexao::receber(char *dest, int max)
{
    int rec=recv(fd,dest,max,0);
    if((rec<0)&(WSAGetLastError()==WSAEWOULDBLOCK))
        rec=0;
	return rec;
}

char* Conexao::pegaIPRemoto()
{
    return inet_ntoa(dest.sin_addr);
}

unsigned short Conexao::pegaPortaRemoto()
{
	return ntohs(dest.sin_port);
}

char* Conexao::pegaIPLocal()
{
    return inet_ntoa(dest.sin_addr);
}

unsigned short Conexao::pegaPortaLocal()
{
	return ntohs(dest.sin_port);
}

unsigned long Conexao::dns(char *end)
{
//  0 - host invalido
    struct hostent *he;
    if((he=gethostbyname(end))==NULL)
        return 0;
    return *(he->h_addr);
}

sockaddr_in* Conexao::pegaInfo()
{
    return &dest;
}

int Conexao::iniTE()
{
    if((callback==NULL)|(!ativa()))
        return 0;
    sairTE=false;
    if((evento=WSACreateEvent())==WSA_INVALID_EVENT)	//nao foi possivel criar objeto
        return -1;
    else
    {
        WSAEventSelect(fd,evento,FD_READ|FD_WRITE|FD_OOB|FD_ACCEPT|FD_CONNECT|FD_CLOSE);
        if((threadEventosHnd=CreateThread(NULL,0,Conexao::threadEventos,(LPVOID)this,0,&idTE))==NULL)
        {
            WSACloseEvent(evento);
            return -2;
        }
    }
    return 0;
}

int Conexao::fimTE()
{
    sairTE=true;
    if(threadEventosHnd!=NULL)
    {
        int retorno=WaitForSingleObject(threadEventosHnd,1000);
//		    if(retorno==WAIT_OBJECT_0)
//		    	printf("\nThread sinalizada normalmente (WAIT_OBJECT_0)\n");
//	    	else
        if(retorno==WAIT_TIMEOUT)
            return -2;  //Thread nao sinalizada (WAIT_TIMEOUT)
        CloseHandle(threadEventosHnd);
        threadEventosHnd=NULL;
    }
    return 0;
}

int Conexao::registraCallback(int(*fn)(Conexao*,long,long[]))
// -1 : erro de alocacao de evento
// -2 : erro ao criar/finalizar thread
{
	if(callback!=NULL)
	{
        if(fimTE()!=0)
            return -2;
        callback=NULL;
	}
	if(fn!=NULL)
	{
        callback=fn;
        if(ativa())
            return iniTE();
	}
	return 0;
}

int Conexao::registraCallbackFilho(int(*fn)(Conexao*,long,long[]))
{
	callbackfilho=fn;
	return 0;
}


DWORD WINAPI Conexao::threadEventos(LPVOID este)
{
    Conexao *c=(Conexao*)este;
    WSANETWORKEVENTS atual;
    while(!c->sairTE)
    {
		if(WSAWaitForMultipleEvents(1,&c->evento,false,100,false)!=WSA_WAIT_TIMEOUT)
		{
			WSAEnumNetworkEvents(c->fd,c->evento,&atual);
			if(!WSAResetEvent(c->evento))
				break;
			if(c->callback==NULL)
				break;
			//se callback retornar valor != 0 mata o  thread
			if(c->callback(c,atual.lNetworkEvents,(long*)atual.iErrorCode))
				break;
		}
    }
	WSACloseEvent(c->evento);
    if(c->fd!=INVALID_SOCKET)
    {
        closesocket(c->fd);
        c->fd=INVALID_SOCKET;
    }
    return 0;
}
#endif
