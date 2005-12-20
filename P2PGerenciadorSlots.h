#ifndef MBSUTIL_P2PGERENCIADORSLOTS_H
#define MBSUTIL_P2PGERENCIADORSLOTS_H

#include "P2PSlot.h"

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
		- broadcast e pesquisa
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
	virtual int IPH_conectado(const Noh& n) = 0;
	virtual int IPH_desconectado(const Noh& n) = 0;
};

class GerenciadorSlots : public iFrameHandler
{
public:
	Cliente iC;
	ListaNohs nohs;

	GerenciadorSlots(int num=10);
	virtual ~GerenciadorSlots();

	void registraPacketHandler(iPacketHandler *pai)
		{ ph=pai; }

	int pegaNumSlots() const { return numSlots; }
	int mudaNumSlots(int num);

	virtual int IFH_tratar(Buffer *frame, Slot *slot);
	virtual int IFH_conectado(Slot *slot);
	virtual int IFH_desconectado(Slot *slot);

	Slot* at(int num) const;
	Slot* operator[](int i) const { return &slots[i]; }

	Slot* operator[](const Hash128& user) const;	//busca
	Slot* operator[](const Noh& n) const;			//busca

	int aloca();
	//TODO:	int libera();	//mata slot com a classificacao mais baixa e aloca

	int enviar(Buffer *pkt, const Noh& n);
	int ouvir(unsigned short porta=0);	//0=iC.porta
	int conectar(const Noh& n);
	int conectar(const char *ip, const unsigned short porta);
	int conectar(std::string ip, const unsigned short porta);
	int desconectar(int nslot=-1);	//-1 = todos
	void ping();					//manda pedido de ping p/ todos slots
private:
	Mutex m;
	int numSlots;
	Slot *slots;
	iPacketHandler *ph;
	friend class ClienteP2P;
	Conexao serverSock;
	static int tratarServer(Conexao *con, long codeve, long coderro[]);
};
#endif
