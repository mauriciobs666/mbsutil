#ifndef RedeH
#define RedeH

#include <winsock2.h>

class Buffer
{
    public:
        unsigned char *dados,*pntE,*pntL;
        int ocupados;

        Buffer();
        Buffer(int tam);
        unsigned long pegaTamanho();
        int mudaTamanho(int tam);
        ~Buffer();

        void resetPnt();        //pntL=dados,pntE=(dados+ocupados)
        int faltaTratar();      //pntE-pntL
        int livres();
        int append(Buffer *b);  //anexa faltaTratar() bytes de b->pntL em pntE
        int append(Buffer *b, int qtd); //anexa qtd bytes de b->pntL em pntE
    protected:
        int tamanho;
        void mata(unsigned char *&end);
        int aloca(unsigned char *&end, int qtd);
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
        int ouvir(unsigned short porta); //backlog=10
        int ouvir(unsigned short porta, int backlog);
        Conexao* aceitar();
        void recusar();

    protected:

        //win32 socket
        sockaddr_in dest;
        int fd;
        int criaSocket();
        int abreSocket();
        int fechaSocket();

        //Loop de eventos e callback
        WSAEVENT evento;
        static DWORD WINAPI threadEventos(LPVOID este);
        HANDLE threadEventosHnd;
        DWORD idTE;
        int (*callback)(Conexao*,long,long[]);
        int (*callbackfilho)(Conexao*,long,long[]);
        int iniTE();    //inicia thread de eventos
        int fimTE();    //finaliza thread de eventos
        bool sairTE;
};
#endif

