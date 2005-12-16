#ifndef MBSUTIL_PROTOCOLO_H
#define MBSUTIL_PROTOCOLO_H

#include <cstdio>
#include <string>
#include <map>
#include <list>
#include <queue>
#include <set>
#include <iostream>

#include "Hash.h"
#include "Bufer.h"
#include "Thread.h"
#include "Rede.h"
#include "Parser.h"

/*	Descricao do protocolo P2P

	Camada 0 (enlace):

	Responsavel pelo envio e recebimento de pacotes, implementada em class Slot.
	Formato:
		[TAMANHO][dados]
			TAMANHO = typedef = tamanho total da area de dados
			dados  = TAMANHO * bytes = pacote camada 1
*/
namespace Protocolo
{
	typedef unsigned short TAMANHO;
	typedef unsigned short COMANDO;

	typedef enum CmdCamada1
	{
		/*
			Camada 1 (rede):
				Responsavel pelo roteamento de pacotes, descobrimento de hosts, pesquisa,
				balanceamento da rede
			Formato:
				[COMANDO][dados]
					COMANDO = typedef = enum Protocolo::CmdCamada1
					dados = depende do COMANDO
		*/
		PING,
		/*	Ping
			[unsigned long timestamp]
		*/
		PONG,
		/*	Resposta ao ping
			[unsigned long timestamp] (copia do recebido em ping)
		*/
		ROTEAR,
		/*	Pedido de roteamento do pacote pra cliente com id baixa
			[remetente][destinatario][dados]
			[Noh][Hash128]
		*/
		ROTEAR_ERRO,
		/*	Nao foi possivel rotear pacote (Usuario desconectado)
		*/
		ROTEAR_BLOQUEADO,
		/*	Cliente nao aceita rotear este comando
		*/
		RETORNO,
		/*	Pedido de conexao de callback
		*/
		BUSCAR,
		/*	Broadcast de busca
			[unsigned char TTL][criterio]
		*/
		HIT,
		/*
			Encontrado
		*/
		ACK,
		/*	Acknowledge (pra chat p.e)
		*/
		LOGIN,
		/*	pacote com informacoes sobre o cliente e usuario
			[infoCliente][Usuario]
				infoCliente = [versao][opcoes][Noh][mtu][mru]
					Noh = [ip][porta][id]
						sizeof(Noh)=10
					sizeof(infoCliente)=12+sizeof(Noh)=22
				infoUsuario = [Hash128][Nick]
					sizeof(infoUsuario)=16+TAMNICK=32
			sizeof(dados)=54
		*/
		MENSAGEM,
		/*	Mensagem instantanea (texto/chat)
			[char[] (nao-ASCIIZ)]
		*/
	};

	typedef enum CmdCamada2
	{
		USER,
		/*	protocolo definido pelo usuario
			[unsigned char codigo do protocolo][dados]
		*/
	};

	//	Constantes diversas
	const int PORTA_PADRAO=6661;
	const int TAMNICK=15+1;
	const int VERSAOINFO=1;		//versao dos arquivos

	typedef enum
	{
		ERRO_IO,		//erro de leitura/escrita
		ERRO_VERSAO,	//versao incorreta do arquivo
		OK,				//tudo ok
	} IOArquivo;
};

using std::istream;
using std::ostream;

/*	Classe Noh
	Responsavel por armazenar informacoes sobre um endereco de rede TCP/IP.
*/
class Noh
{
public:
//TODO:	unsigned long codigo;	//associacao com Usuario
	unsigned long ip;
	unsigned short porta;
	unsigned long id;

	Noh();
	bool idAlta() const
		{ return ((ip!=0)?(ip==id):false); }
	bool operator==(const Noh& n) const
		{ return ((ip==n.ip)&(porta==n.porta)&(id==n.id)); }
	bool operator<(const Noh& n) const
		{
			return (ip<n.ip) ? true : (ip>n.ip ? false :
				(porta<n.porta) ? true : (porta>n.porta ? false :
				(id<n.id) ? true : false) );
		}
	int read(unsigned char *&pkt);
	int write(unsigned char *&pkt) const;
    istream& read(istream& is);
    ostream& write(ostream& os) const;
};

/*	Classe Cliente
	Responsavel por descrever um cliente da rede P2P, protocolos aceitos etc.
*/
class Cliente : public Noh
{
public:
	unsigned long versao;
	unsigned long opcoes;
	unsigned short mtu;
	unsigned short mru;
	unsigned long ping;

	Cliente();
	int read(unsigned char*& pkt);
	int write(unsigned char*& pkt) const;
	istream& read(istream& is);
    ostream& write(ostream& os);
};

/*	Classe Usuario
	Descreve um usuario da rede
*/
class Usuario : public Hash128
{
public:
	char nick[Protocolo::TAMNICK];	//TODO: nick deveria ser string...
	unsigned long rx;				//volume de dados recebidos
	unsigned long tx;				//volume de dados enviados

	int classificacao() const
	{
		return rx-tx;
		//amigo=k
		//classificacao=saldo+amigo
	}

	unsigned long codMsg;			//id sequencial das mensagens (ACK)
	unsigned long codCli;			//codigo do noh (ou cliente) na lista de Nohs

	Usuario();
	Usuario(const Usuario& base);
	int mudaNick(std::string novo);

	int read(unsigned char*& pkt);
	int write(unsigned char*& pkt) const;
    istream& read(istream& is);
    ostream& write(ostream& os);
};

/*	Classe Slot
	Responsavel pela camada 0 do protocolo e toda parte de baixo nivel da
	comunicacao, envio/recepcao de pacotes, gerenciamento da conexao etc.
*/
class Slot
{
public:
	friend class ClienteP2P;
	friend class ArraySlots;

	Cliente iC;
	Usuario iU;	//TODO: protejer e transformar em * ou usar soh o hash
	//TODO: timestamps

	Slot();
	~Slot();
	int desconectar();

	typedef enum
	{
		LIVRE = 0,		//desconectado
		ESPERANDO,		//desconectado, mas reservado pelo gerenciador
		LOGIN,			//conectado mas esperando login
		CONECTADO		//conectado normal
	} EstadoSlot;
	EstadoSlot pegaEstado();
	int setaEstado(EstadoSlot estado);

	int enviar(Buffer *pkt);
	Buffer* receber();
protected:
	Mutex m;
	Conexao *c;
	EstadoSlot estado;

	int _reset();
	bool _conectado();

	//Fila de recepcao
	std::queue<Buffer*> recebidos;	//fila de pacotes prontos

	//Maquina de estados para RX de Slot
	Buffer temp;					//usado na recepcao de Conexao
	Protocolo::TAMANHO tamRecebendo;//tamanho do pacote "*recebendo"
	Buffer *recebendo;				//pacote sendo recebido
	enum EstadosRX
	{
		NOVO,			//esperando novo pacote
		ESPERA_TAMANHO,	//esperando segundo byte de tamanho
		DADOS,			//recebendo dados
	} estadoRX;
};

/*	Classe ArraySlots
	Gerencia recursos de rede, limita o max de conexoes simultaneas, aloca e
	libera Slots
*/
class ArraySlots
{
public:
	ArraySlots(int num=10);
	~ArraySlots();
	int pegaNumSlots() const { return numSlots; }
	int mudaNumSlots(int num);
	Slot* at(int num) const;
	Slot* operator[](int i) const { return &slots[i]; }
	Slot* operator[](const Hash128& user) const;	//busca
	int aloca();
	//TODO:	int libera();	//mata slot com a classificacao mais baixa e aloca
	int conectar(const char *ip, const unsigned short porta);
	int conectar(std::string ip, const unsigned short porta);
	int desconectar(int nslot=-1);	//-1 = todos
protected:
	Mutex m;
	int numSlots;
	Slot *slots;
};

/*	Classe ListaHash128
	Fornece acesso facil e sincronizado para listas de hashs
	IMPORTANTE: acessos diretos devem ser protegidos pelo mutex
*/
class ListaHash128
{
public:
	Mutex m;
	std::set<Hash128> lista;

	ListaHash128() {}
	~ListaHash128() { limpa(); }

	int insere(const Hash128& h);
	void remove(const Hash128& h);
	bool busca(const Hash128& h);
	int tamanho();
	void limpa();
    istream& read(istream& is);
    ostream& write(ostream& os);
};

/*	Classe ListaUsuarios
	Fornece acesso facil e sincronizado para listas de usuarios
	IMPORTANTE: acessos diretos devem ser protegidos pelo mutex
*/
class ListaUsuarios
{
public:
	Mutex m;
	std::map<Hash128,Usuario*> lista;

	ListaUsuarios() {}
	~ListaUsuarios() { limpa(); }

	bool insere(const Hash128& h, Usuario* u);
	void remove(const Hash128& h);
	Usuario* busca(const Hash128& h);
	int tamanho();
	void limpa();
    istream& read(istream& is);
    ostream& write(ostream& os);
};

class ListaNohs
{
public:
	void push(const Noh& no);
	Noh* pop(const Noh& no);
	int tamanho();
	void limpa();
    istream& read(istream& is);
    ostream& write(ostream& os);
private:
	Mutex m;
	std::list<Noh> lista;
};

class EventoP2PUI
{
public:
	const enum tipoEvento
	{
		RELATORIO,		//[mensagem] deve ser colocada na area de log
		MSG_CHAT_RX,	//[usuario] enviou [mensagem] de chat
		MSG_CHAT_TX		//envio da mensagem [id] concluido
	} tipo;
	unsigned long id;
	bool erro;			//indica erro
	Hash128 usuario;
	std::string mensagem;

	EventoP2PUI(std::string& msg)
		: tipo(RELATORIO), erro(false), mensagem(msg) {}
};

/*	Classe ClienteP2PUI
	Interface do responsavel pelo tratamento de eventos do cliente p2p (callback).
	Deve ser herdada e reimplementada.
*/
class ClienteP2PUI
{
public:
	virtual void relatorio(const std::string& mensagem)
		{ std::cerr << mensagem << std::endl; }
	virtual void mostraMensagemChat(const Hash128* remetente, const std::string& mensagem)
		{ std::cerr << remetente->toString() << ":" << mensagem << std::endl; }
//	virtual void trataEvento(EventoP2PUI* evento);
};

class ClienteP2P
{
public:
	ArraySlots slots;
	Cliente iC;
	Usuario iU;

	ListaUsuarios usuarios;		//arvore com todos usuarios conhecidos
	ListaHash128 amigos;		//lista de amigos
	ListaHash128 blacklist;		//lista negra
	ListaNohs nohs;

	ClienteP2P();
	~ClienteP2P();
	void registraUI(ClienteP2PUI *ui);	//registra callbacks de UI
	int abrir(std::string dir=".");		//abre todas as listas e info
	int salvar(std::string dir=".");	//salva todas as listas e info
	Protocolo::IOArquivo abrirID(std::string arquivo);
	Protocolo::IOArquivo salvarID(std::string arquivo);
	int esperarConexoes(unsigned short porta=0);	//0=iC.porta
	int enviarMsg(std::string msg, const Hash128* user);
	int enviarMsg(const char *msg, const Hash128* user);
	void ping();						//manda pedido de ping p/ todos slots
protected:
	int enviaLogin(Slot *slot);
	int enviaPing(Slot *slot);
	int enviaPong(unsigned long timestamp, Slot *slot);
	int tratar(Buffer *pacote, Slot *slot);		//trata pacote recebido
private:
	Conexao serverSock;
	static int tratar(Conexao *con, long codeve, long coderro[]);
	static int tratarServer(Conexao *con, long codeve, long coderro[]);
};
#endif
