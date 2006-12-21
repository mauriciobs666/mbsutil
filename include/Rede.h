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

#ifndef MBSUTIL_REDE_H
#define MBSUTIL_REDE_H

#include <cstdlib>
#include <string>
#include <winsock2.h>
#include "Thread.h"

//using namespace std;

class Soquete
{
public:
    Soquete();
    Soquete(int winfd);
    virtual ~Soquete();

	virtual bool conectado();
    virtual int conectar(std::string ip, unsigned short porta);
    virtual void desconectar();
    virtual int enviar(char *dados, int len);
    virtual int receber(char *dest, int max);

	std::string IPRemoto();
	std::string IPLocal();
    unsigned short PortaRemoto();
	unsigned short PortaLocal();
    sockaddr_in* pegaInfo();
    unsigned long dns(std::string end);
	std::string toString(unsigned long ip);
protected:
    //win32 socket
    unsigned fd;
	sockaddr_in dest;
    unsigned criaSocket();
    unsigned fechaSocket();
};

class SoqueteServer : protected Soquete
{
public:
    virtual int ouvir(unsigned short porta, int backlog=10);
    virtual Soquete* aceitar();
    virtual void recusar();
};

/*
    ------------------------
    Estruturas de winsock2.h
    ------------------------

    struct sockaddr_in
    {
        short int sin_family;           // sempre AF_INET
        unsigned short int sin_port;    // Porta
        struct in_addr sin_addr;        // IP (ver abaixo)
        unsigned char sin_zero[8];      // Padding
    };

    struct in_addr
    {
        unsigned long s_addr;           // Endereço IP (4 bytes)
    };
*/

class Conexao
{
public:
	//comuns a cliente e servidor
	Conexao();
	~Conexao();
	bool ativa();
	int registraCallback(int(*fn)(Conexao*,long,long[]));
	char* pegaIPRemoto();
	unsigned short pegaPortaRemoto();
	char* pegaIPLocal();
	unsigned short pegaPortaLocal();
	void desconectar();
	sockaddr_in* pegaInfo();
	unsigned long dns(char *end);

	//uso livre por objetos gerenciadores, callbacks etc
	unsigned long id;       //default=0
	void *pai;              //default=NULL

	//exclusivo p/ conexoes cliente
	int conectar(const char *ip, const unsigned short porta);
	int conectar(unsigned long end, unsigned short porta);
	int enviar(char *dados, int len);
	int receber(char *dest, int max);

	//exclusivo p/ conexoes servidor
	Conexao(int soquete);
	int registraCallbackFilho(int(*fn)(Conexao*,long,long[]));
	int ouvir(unsigned short porta, int backlog=10);
	Conexao* aceitar();
	void recusar();

protected:

	//win32 socket
	sockaddr_in dest;
	unsigned fd;

	//Loop de eventos e callback
	WSAEVENT evento;
	int (*callback)(Conexao*,long,long[]);
	int (*callbackfilho)(Conexao*,long,long[]);

	Thread *eventos;
	bool eventosSair;
	static void* eventosFunc(void* esta);

	static DWORD WINAPI threadEventos(LPVOID este);
	HANDLE threadEventosHnd;
	DWORD idTE;
	int iniTE();    //inicia thread de eventos
	int fimTE();    //finaliza thread de eventos
	bool sairTE;
};
#endif

