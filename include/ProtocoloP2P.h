#ifndef MBSUTIL_PROTOCOLO_H
#define MBSUTIL_PROTOCOLO_H

#include "P2PGerenciadorSlots.h"
#include <ctime>

using std::istream;
using std::ostream;

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
	int enviarMsg(std::string msg, const Hash128* user);
	int enviarMsg(const char *msg, const Hash128* user);
protected:
	int enviaLogin(const Noh& n);
	virtual int IPH_tratar(Buffer *pacote, Slot *slot);		//trata pacote recebido
	virtual int IPH_conectado(const Noh& n);
	virtual int IPH_desconectado(const Noh& n);
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
