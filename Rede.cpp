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
		if(WSAStartup(MAKEWORD(1,1),&wsad)==0)
		{
			inicializado=true;
			atexit(&fimRede);
		}
	}
};

using namespace std;

//------------------------------------------------------------------------------
//      Soquete
//------------------------------------------------------------------------------

Soquete::Soquete()
{
	if(!inicializado)
		iniciaRede();
	fd=INVALID_SOCKET;
}

Soquete::Soquete(int winfd)
{
	fd=winfd;
}

Soquete::~Soquete()
{
    desconectar();
}

int Soquete::conectar(string ip, unsigned short porta)
//  0: tudo ok, conectado
// -1: erro generico de conexao
// -2: erro de dns, host naum encontrado
{
	if(conectado())
		return -1;

	struct hostent *he;
    if((he=gethostbyname(ip.c_str()))==NULL)
        return -2;
    dest.sin_addr=*((in_addr*)he->h_addr);

    dest.sin_port=htons(porta);
    dest.sin_family=AF_INET;
    memset(&(dest.sin_zero),0,8);

    if(INVALID_SOCKET==criaSocket())
    	return -1;

	if(connect(fd,(sockaddr*)&dest,sizeof(sockaddr))==-1)
        if(WSAGetLastError()!=WSAEWOULDBLOCK)
            return -1;

    return 0;
}

bool Soquete::conectado()
{
    return (fd==INVALID_SOCKET) ? false : true;
}

void Soquete::desconectar()
{
    if(conectado())
		fechaSocket();
}

int Soquete::enviar(char *dados, int len)
{
	int rc=send(fd,dados,len,0);
	if(rc<=0)
		fechaSocket();
    return rc; 
}

int Soquete::receber(char *dest, int max)
{
	int rc=recv(fd,dest,max,0);
	if(rc<=0)
		fechaSocket();
    return rc; 
}

string Soquete::IPRemoto()
{
	return toString(dest.sin_addr.s_addr);
}

string Soquete::IPLocal()
{
	//TODO: IPLocal()
    return "";
}

unsigned short Soquete::PortaRemoto()
{
	return ntohs(dest.sin_port);
}

unsigned short Soquete::PortaLocal()
{
	//TODO: PortaLocal()
    return 0;
}

sockaddr_in* Soquete::pegaInfo()
{
	return &dest;
}

unsigned long Soquete::dns(string end)
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

string Soquete::toString(unsigned long ip)
{
	return string(inet_ntoa(*((in_addr*)&ip)));
}

unsigned Soquete::criaSocket()
{
	return fd=socket(AF_INET,SOCK_STREAM,0);
}

unsigned Soquete::fechaSocket()
{
	closesocket(fd);
    fd=INVALID_SOCKET;
    return fd;
}

//------------------------------------------------------------------------------
//      SoqueteServer
//------------------------------------------------------------------------------

int SoqueteServer::ouvir(unsigned short porta, int backlog)
//	-1 : erro generico de conexao
//	-2 : erro de bind, porta em uso
{
	if(conectado())
		return -1;

    if(INVALID_SOCKET==criaSocket())
    	return -1;

	dest.sin_family=AF_INET;
	dest.sin_port=htons(porta);
	dest.sin_addr.s_addr=INADDR_ANY;
	memset(&(dest.sin_zero),0,8);
	
	if(bind(fd,(sockaddr*)&dest,sizeof(sockaddr))==-1)
	{
		fechaSocket();
		return -2;
	}
	if(listen(fd,backlog)==-1)
	{
		fechaSocket();
		return -2;
	}
    return 0;
}

Soquete* SoqueteServer::aceitar()
{
	Soquete *novo;
    sockaddr_in adn;
    unsigned fdn=accept(fd,(sockaddr*)&adn,&sin_size);
    if(fdn==INVALID_SOCKET)
        return NULL;
    novo=new Soquete(fdn);
    return novo;
}

void SoqueteServer::recusar()
{
    sockaddr_in adn;
    closesocket(accept(fd,(sockaddr*)&adn,&sin_size));
}

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
    estadoTX=0;
    estadoRX=0;
	callback=NULL;
    threadEventosHnd=NULL;
}

Conexao::Conexao(int soquete)
{
    pai=NULL;
    id=0;
    estadoTX=0;
    estadoRX=0;
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
    dest.sin_addr=*((in_addr*)he->h_addr);
    dest.sin_port=htons(porta);
    dest.sin_family=AF_INET;
    memset(&(dest.sin_zero),0,8);
    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==INVALID_SOCKET)
    	return -1;
    estadoTX=0;
    estadoRX=0;
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
	return recv(fd,dest,max,0);
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
