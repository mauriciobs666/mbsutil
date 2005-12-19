#ifndef MBSUTIL_PROTOCOLO_H
#define MBSUTIL_PROTOCOLO_H

#include "Bufer.h"
#include "Rede.h"
#include "Parser.h"
#include "P2Ped.h"

using std::istream;
using std::ostream;

/*  ===========================================================================
	Classe Slot - Camada 0 do protocolo (enlace)
	===========================================================================

	Responsavel por:
		- toda parte de baixo nivel da comunicacao
		- envio/recepcao de frames
		- deteccao e tratamento erros de:
			- sincronismo
			- queda de conexao
			- time out

	Formato dos frames:
		[TAMANHO][dados]
			TAMANHO = typedef = tamanho total da area de dados
			dados  = TAMANHO * bytes = pacote camada 1

	===========================================================================
*/

class iFrameHandler	// interface do tratador de frames
{
public:
	virtual int IFH_tratar(Buffer *frame, class Slot *slot) = 0;
	virtual int IFH_conectado(Slot *slot) = 0;
	virtual int IFH_desconectado(Slot *slot) = 0;
};

class Slot
{
public:
	friend class ClienteP2P;
	friend class GerenciadorSlots;

	Cliente iC;
	Usuario iU;	//TODO: protejer e transformar em * ou usar soh o hash
	//TODO: timestamps p/ time-out

	Slot();
	~Slot();
	void registraFrameHandler(iFrameHandler *pai)
		{ gerenciador=pai; }

//	int conectar(const char *ip, const unsigned short porta);
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
	iFrameHandler *gerenciador;	//gerenciador de slots
	Mutex m;
	Conexao *c;
	EstadoSlot estado;

	int _reset();		//desprotegida
	bool _conectado();	//desprotegida

	//Fila de recepcao
	std::queue<Buffer*> recebidos;	//fila de frames prontos pra serem tratados

	//Maquina de estados para RX de Slot
	Buffer temp;					//usado na recepcao de Conexao
	Protocolo::TAMANHO tamRecebendo;//tamanho temporario do frame "*recebendo"
	Buffer *recebendo;				//frame sendo recebido

	enum EstadosRX
	{
		NOVO,			//esperando novo frame
		ESPERA_TAMANHO,	//esperando segundo byte de tamanho
		DADOS,			//recebendo dados
	} estadoRX;
private:
	static int tratar(Conexao *con, long codeve, long coderro[]);
};

/*	===========================================================================
	Classe GerenciadorSlots - Camada 1 fo protocolo (rede):
	===========================================================================

	Responsavel por:
		- camada 1 do protocolo
		- gerenciamento de conexoes e outros recursos de rede
		- limite de max de conexoes simultaneas
		- alocacao e liberacao de Slots
		- enviar e receber pacotes
		- roteamento
		- descobrimento de hosts
		- pesquisa
		- balanceamento da rede
		- select() ???

	Formato dos pacotes:
		[COMANDO][dados]
			COMANDO = typedef = enum Protocolo::CmdCamada1
			dados = depende do COMANDO

	===========================================================================
*/
class iPacketHandler	// interface do tratador de pacotes
{
public:
	virtual int IPH_tratar(Buffer *pacote, Slot *slot) = 0;
	virtual int IPH_conectado(Slot *slot) = 0;
	virtual int IPH_desconectado(Slot *slot) = 0;
};

class GerenciadorSlots : public iFrameHandler
{
public:
	ListaNohs nohs;

	GerenciadorSlots(int num=10);
	virtual ~GerenciadorSlots();

	void registraPacketHandler(iPacketHandler *pai)
		{ ph=pai; }

	virtual int IFH_tratar(Buffer *frame, Slot *slot);
	virtual int IFH_conectado(Slot *slot);
	virtual int IFH_desconectado(Slot *slot);

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
private:
	Mutex m;
	int numSlots;
	Slot *slots;
	iPacketHandler *ph;
	friend class ClienteP2P;
	Conexao serverSock;
	static int tratarServer(Conexao *con, long codeve, long coderro[]);
};

/*	Classe ClienteP2PUI
	Interface do responsavel pelo tratamento de eventos do cliente p2p.
*/
class ClienteP2PUI
{
public:
	virtual void relatorio(const std::string& mensagem)
		{ std::cerr << mensagem << std::endl; }
	virtual void mostraMensagemChat(const Hash128* remetente, const std::string& mensagem)
		{ std::cerr << remetente->toString() << ":" << mensagem << std::endl; }
};

class ClienteP2P : public iPacketHandler
{
public:
	GerenciadorSlots slots;
	Cliente iC;
	Usuario iU;

	ListaUsuarios usuarios;		//arvore com todos usuarios conhecidos
	ListaHash128 amigos;		//lista de amigos
	ListaHash128 blacklist;		//lista negra

	ClienteP2P();
	virtual ~ClienteP2P();
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
	virtual int IPH_tratar(Buffer *pacote, Slot *slot);		//trata pacote recebido
	virtual int IPH_conectado(Slot *slot);
	virtual int IPH_desconectado(Slot *slot);
};
#endif

/*	A ser implementado um belo dia ...
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
//	virtual void trataEvento(EventoP2PUI* evento);
*/
