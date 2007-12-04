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

#ifndef MBSSLOT_H
#define MBSSLOT_H

#include "MBSNetwork.h"
#include "Bufer.h"
#include <queue>

typedef unsigned short TAMANHO;

class MBSSlot
{
	public:
		time_t timestamp;	//!<time-out rx

		MBSSlot() : sock(NULL), temp(500), recebendo(NULL)
			{ reset(); }
		virtual ~MBSSlot()
			{ reset(); }
		void reset();

		int conectar(const char *ip, const unsigned short porta);
		int conectar(MBSSocket *s);
		int desconectar()
			{ reset(); }

		//!Estado do Slot
		typedef enum EstadoSlot
		{
			LIVRE = 0,		//!<desconectado
			RESERVADO,		//!<desconectado, mas reservado pelo gerenciador
			HANDSHAKE,		//!<conectado, descobrindo ip, id etc
			LOGIN,			//!<conectado, esperando login
			CONECTADO		//!<conectado, normal
		};
		EstadoSlot pegaEstado();
		int setaEstado(EstadoSlot estado);

		int enviar(Buffer *pkt);
		Buffer* receber();

		SOCKET getFD()
			{ return (sock==NULL) ? INVALID_SOCKET : sock->fd; }

	private:
		MBSSocket *sock;
		EstadoSlot estado;

//		bool _conectado();	//desprotegida

		Buffer temp;					//usado na recepcao de Conexao
		TAMANHO tamRecebendo;           //tamanho temporario do frame "*recebendo"
		Buffer *recebendo;				//frame sendo recebido
		std::queue<Buffer*> recebidos;	//fila de frames prontos pra serem tratados

		//!Estado de RX do pacote
		enum EstadosRX
		{
			NOVO,			//!<esperando novo frame
			ESPERA_TAMANHO,	//!<esperando segundo byte de tamanho
			DADOS,			//!<recebendo dados
		} estadoRX;
};

#endif // MBSSLOT_H
