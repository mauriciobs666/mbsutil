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

#ifndef MBSUTIL_P2PGERENCIADORSLOTS_H
#define MBSUTIL_P2PGERENCIADORSLOTS_H

#ifdef _WIN32

#include "P2PSlot.h"

//!Interface do tratador de pacotes
class iPacketHandler
{
public:
	virtual int IPH_tratar(Buffer *pacote, const Noh& n) = 0;
	virtual int IPH_conectado(const Noh& n) = 0;
	virtual int IPH_desconectado(const Noh& n) = 0;
	virtual ~iPacketHandler() {}
};

/**
	\brief Camada 1 do protocolo (rede):

	Responsavel por:
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
*/
class P2PGerenciadorSlots : public iFrameHandler
{
public:
	Cliente iC;
	ListaNohs nohs;
//TODO:	ListaNohs roteadores;	//lista de roteadores disponiveis

	P2PGerenciadorSlots(int num=10);
	virtual ~P2PGerenciadorSlots();

	void registraPacketHandler(iPacketHandler *pai)
		{ ph=pai; }

	int pegaNumSlots() const { return numSlots; }
	int mudaNumSlots(int num);

	virtual int IFH_tratar(Buffer *frame, P2PSlot *slot);
	virtual int IFH_conectado(P2PSlot *slot);
	virtual int IFH_desconectado(P2PSlot *slot);

	P2PSlot* at(int num) const;
	P2PSlot* operator[](int i) const { return &slots[i]; }
	P2PSlot* operator[](const Noh& n) const;			//busca

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
	SessaoCritica cs;
	int numSlots;
	P2PSlot *slots;
	iPacketHandler *ph;
	//friend class ClienteP2P;
	Conexao serverSock;
	static int tratarServer(Conexao *con, long codeve, long coderro[]);
	bool idAlta(const Noh &n);
};

#endif
#endif
