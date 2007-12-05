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

#include "MBSSlot.h"
#include <iostream>

namespace
{
//	const int TAMBUFFER=500;	//Buffer temp
	const int TIMEOUT_RX=60;	//timeout de recepcao de frames (em segundos)
};

using namespace std;

//------------------------------------------------------------------------------
//      MBSSlot
//------------------------------------------------------------------------------

void MBSSlot::reset()
{
	estado=LIVRE;
	estadoRX=NOVO;
	if(sock!=NULL)
	{
        delete sock;
		sock=NULL;
	}
    if(recebendo!=NULL)
    {
        delete recebendo;
        recebendo=NULL;
    }
	while(!recebidos.empty())
	{
		delete recebidos.front();
		recebidos.pop();
	}
}

int MBSSlot::conectar(MBSSocket *s)
{
	estado=RESERVADO;
	sock=s;
	timestamp=time(NULL);
	if(sock==NULL)
		estado=LIVRE;
	return 0;
}

int MBSSlot::conectar(const char *ip, const unsigned short porta)
{
	estado=RESERVADO;
	if(sock==NULL)
		sock=new MBSSocket();
	timestamp=time(NULL);
	return sock->conectar(ip,porta);
}

MBSSlot::EstadoSlot MBSSlot::pegaEstado()
{
//	_conectado();
	return estado;
}

MBSSlot::EstadoSlot MBSSlot::setaEstado(EstadoSlot novo)
{
    return estado=novo;
}

int MBSSlot::enviar(Buffer *pkt)
{
	int retorno=-1;
//	_conectado();
    if(estado>RESERVADO)
    {
        TAMANHO tam=pkt->pegaTamanho();
        if(sock->enviar((char*)&tam,sizeof(TAMANHO))==sizeof(TAMANHO)) //envia tamanho
            if(sock->enviar((char*)pkt->dados,tam)==tam)       		//envia pacote
                retorno=0;                                  		//tudo ok
    }
    return retorno;
}

Buffer* MBSSlot::receber()
{
	int qtd;
	do
	{
        temp.reset();
		qtd=sock->receive((char*)temp.pntE, temp.livres());
//		cout << "qtd=" << qtd << endl;
		if(qtd==0)
		{
			reset();
			return NULL;
		}
		//TODO: check for qtd<0
		if(qtd>0)
			temp.pntE+=qtd;
		while(temp.disponiveis()>0)
		{
			switch(estadoRX)
			{
				case NOVO:
					//esperando primeiro byte de tam do novo pacote
					*((char*)&tamRecebendo)=*temp.pntL;
//					cout << " tamB1 =" << (int)*temp.pntL;
					temp.pntL++;
					estadoRX=ESPERA_TAMANHO;
//					cout << " NOVO ";
				break;
				case ESPERA_TAMANHO:
					//esperando segundo byte de tamanho
					*(((char*)(&tamRecebendo))+1)=*temp.pntL;
//					cout << " tamB1 =" << (int)*temp.pntL;
					temp.pntL++;
					//tamanho recebido, agora o pacote em si
					recebendo=new Buffer(tamRecebendo);
					estadoRX=DADOS;
//					cout << " ESPERA_TAMANHO tamRecebendo=" << tamRecebendo;
				break;
				case DADOS:
					//receber comando e dados
//					cout << " DADOS ";
					recebendo->append(temp,recebendo->livres());
//					cout << " livres = " << recebendo->livres();
					if(recebendo->livres()==0)	//aceitacao
					{
						#ifdef LOGAR_SOCKET
							logar("pacote completo recebido");
						#endif
						recebidos.push(recebendo);
						recebendo=NULL;
						estadoRX=NOVO;
						timestamp=time(NULL);
					}
				break;
				default:
					cout << __FILE__ << ":" << __LINE__ << ":IMPOSSIVEL" << endl;
				break;
			}
		}
	} while(qtd>0);
	Buffer *retorno=NULL;
	if(!recebidos.empty())
	{
		retorno=recebidos.front();
		recebidos.pop();
	}
	return retorno;
}
/*
bool MBSSlot::_conectado()
{
	if(c!=NULL)
	{
		if(!c->ativa())
			_reset();
		else
		{
			time_t agora=time(NULL);
			if((agora-timestamp)>TIMEOUT_RX)
				_reset();
			else
				return true;
		}
	}
	else
		_reset();
	return false;
}
*/
