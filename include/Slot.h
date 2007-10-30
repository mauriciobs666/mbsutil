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

#ifndef MBSUTIL_P2PSLOT_H
#define MBSUTIL_P2PSLOT_H

#include "Bufer.h"
#include "Rede.h"
#include "P2Ped.h"

//!Interface do tratador de frames
class iFrameHandler
{
public:
	virtual int IFH_tratar(Buffer *frame, class Slot *slot) = 0;
	virtual int IFH_conectado(Slot *slot) = 0;
	virtual int IFH_desconectado(Slot *slot) = 0;
	virtual ~iFrameHandler() {}
};

/**
	\brief Camada 0 do protocolo (enlace)

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
*/
class Slot
{
public:
	Cliente iC;
	time_t timestamp;	//!<time-out rx

	Slot(iFrameHandler *pai=NULL) : gerenciador(pai), c(NULL), temp(500), recebendo(NULL)
		{ _reset();	}
	~Slot()
		{ _reset(); }
	void registraFrameHandler(iFrameHandler *pai)
		{ gerenciador=pai; }

	int conectar(const char *ip, unsigned short porta);
	int conectar(const Noh& n);
	int conectar(Conexao *con);
	int desconectar();

	//!Estado do Slot
	typedef enum
	{
		LIVRE = 0,		//!<desconectado
		RESERVADO,		//!<desconectado, mas reservado pelo gerenciador
		HANDSHAKE,		//!<conectado, descobrindo ip, id etc
		LOGIN,			//!<conectado, esperando login
		CONECTADO		//!<conectado, normal
	} EstadoSlot;
	EstadoSlot pegaEstado();
	int setaEstado(EstadoSlot estado);

	int enviar(Buffer *pkt);
	Buffer* receber();

protected:
	iFrameHandler *gerenciador;	//!<gerenciador de slots
	Mutex m;
	Conexao *c;
	EstadoSlot estado;

	int _reset();		//desprotegida
	bool _conectado();	//desprotegida

	Buffer temp;					//usado na recepcao de Conexao
	Protocolo::TAMANHO tamRecebendo;//tamanho temporario do frame "*recebendo"
	Buffer *recebendo;				//frame sendo recebido
	std::queue<Buffer*> recebidos;	//fila de frames prontos pra serem tratados

	//!Estado de RX do pacote
	enum EstadosRX
	{
		NOVO,			//!<esperando novo frame
		ESPERA_TAMANHO,	//!<esperando segundo byte de tamanho
		DADOS,			//!<recebendo dados
	} estadoRX;
private:
	static int tratar(Conexao *con, long codeve, long coderro[]);
};
#endif
