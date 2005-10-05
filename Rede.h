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
    ~Soquete();
	
    int conectar(std::string ip, unsigned short porta);
    void desconectar();
    bool conectado();

    int enviar(char *dados, int len);
    int receber(char *dest, int max);
   
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
    unsigned abreSocket();
    unsigned fechaSocket();
};

class SoqueteServer : protected Soquete
{
public:
	//enum Eventos={};
    int ouvir(unsigned short porta, int backlog=10);
    Soquete* aceitar();
    void recusar();
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
	//inicio global do winsock2 (obrigatorio chamar no inicio/fim da aplicacao)
	static int iniciaRede();
	static int finalizaRede();

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
	unsigned char estadoRX; //default=0
	unsigned char estadoTX; //default=0
	void *pai;              //default=NULL

	//exclusivo p/ conexoes cliente
	int conectar(char *ip,unsigned short porta);
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

