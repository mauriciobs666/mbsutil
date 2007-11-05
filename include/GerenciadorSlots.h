/*
	Insano Game Server
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

#ifndef GERENCIADORSLOTS_H_INCLUDED
#define GERENCIADORSLOTS_H_INCLUDED

#include "Slot.h"

/**
	\brief Camada 1 do protocolo (rede):

	Responsavel por:
		- gerenciamento de conexoes e outros recursos de rede
		- limite de max de conexoes simultaneas
		- alocacao e liberacao de Slots
		- enviar e receber pacotes

	Formato dos pacotes:
		[COMANDO][dados]
			COMANDO = typedef = enum Protocolo::CmdCamada1
			dados = depende do COMANDO
*/
class GerenciadorSlots : public iFrameHandler
{
public:
	GerenciadorSlots(int num=10);
	virtual ~GerenciadorSlots();

	int pegaNumSlots() const { return numSlots; }
	int mudaNumSlots(int num);

	virtual int IFH_tratar(Buffer *frame, Slot *slot) = 0;
	virtual int IFH_conectado(Slot *slot) = 0;
	virtual int IFH_desconectado(Slot *slot) = 0;

	Slot* at(int num) const;
	Slot* operator[](int i) const { return &slots[i]; }
	Slot* operator[](std::string id) const;

	int aloca();

	int ouvir(unsigned short porta);
	int desconectar(int nslot=-1);	    //-1 = todos
protected:
	SessaoCritica cs;
	int numSlots;
	Slot *slots;

	Conexao serverSock;
	static int tratarServer(Conexao *con, long codeve, long coderro[]);
};

#endif // GERENCIADORSLOTS_H_INCLUDED
