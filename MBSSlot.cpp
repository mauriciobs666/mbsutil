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

Buffer* MBSSlot::receive()
{
	Buffer *retorno=NULL;
	if(!recebidos.empty())
	{
		retorno=recebidos.front();
		recebidos.pop();
	}
	return retorno;
}

int MBSSlot::receiveLoop()
{
	temp.reset();
	int qtd=sock->receive((char*)temp.pntE, temp.livres());
//	cout << "MBSSlot::receiveLoop qtd=" << qtd << endl;

	if(qtd<=0)
		return qtd;
	else // if(qtd>0)
		temp.pntE+=qtd;
	while(temp.disponiveis()>0)
	{
		switch(estadoRX)
		{
			case NOVO:
			//esperando primeiro byte de tam do novo pacote
				*((char*)&tamRecebendo)=*temp.pntL;
//				cout << " tamB1 =" << (int)*temp.pntL;
				temp.pntL++;
				estadoRX=ESPERA_TAMANHO;
//				cout << " NOVO ";
			break;
			case ESPERA_TAMANHO:
			//esperando segundo byte de tamanho
				*(((char*)(&tamRecebendo))+1)=*temp.pntL;
//				cout << " tamB1 =" << (int)*temp.pntL;
				temp.pntL++;
				//tamanho recebido, agora o pacote em si
				recebendo=new Buffer(tamRecebendo);
				estadoRX=DADOS;
//				cout << " ESPERA_TAMANHO tamRecebendo=" << tamRecebendo;
			break;
			case DADOS:
			//receber comando e dados
//				cout << " DADOS ";
				recebendo->append(temp,recebendo->livres());
//				cout << " livres = " << recebendo->livres();
				if(recebendo->livres()==0)	//aceitacao
				{
					#ifdef LOGAR_SOCKET
						cout << "complete packet received" << endl;
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
	return qtd;
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

//------------------------------------------------------------------------------
//      MBSSlotManager
//------------------------------------------------------------------------------

MBSSlotManager::MBSSlotManager(int num)
{
	slots=NULL;
	numSlots=0;
	mudaNumSlots(num);
}

MBSSlotManager::~MBSSlotManager()
{
    if((slots!=NULL)&&(numSlots>0))
        delete[] slots;
}

int MBSSlotManager::mudaNumSlots(int num)
{
	int rc=0;
	MBSSlot *temp=NULL;

	if(num>64)
	{
		cout << "WARNING: Hit hard-coded limit of 64 sockets." << endl;
		num=64;
	}
	if(num>0)
	{
		temp=new MBSSlot[num];
		if(temp==NULL)
			rc=-1;		//erro de alocacao
	}
	if((slots!=NULL)&&(numSlots>0))
		delete[] slots;

	slots=temp;
	numSlots=num;
	sockSel.clear();

	return 0;
}


MBSSlot* MBSSlotManager::at(int num) const
{
    if((num>=0)&(num<numSlots))
        return &slots[num];
    return NULL;
}

int MBSSlotManager::reserve()
// busca e aloca (estado 0->1) slot livre
// retorna num do slot ou -1 caso todos estejam ocupados
{
	for(int x=0;x<numSlots;x++)
	{
		if(slots[x].pegaEstado()==MBSSlot::LIVRE)
		{
			if(slots[x].setaEstado(MBSSlot::RESERVADO)==0)
				return x;
		}
	}
    return -1;
}

int MBSSlotManager::ouvir(unsigned short porta)
{
	int retorno=0;

	sockSel.remove(sockServer.fd);
	sockServer.disconnect();

	if((retorno=sockServer.ouvir(porta))==0)
		sockSel.add(sockServer.fd);
	return retorno;
}

int MBSSlotManager::desconectar(int nslot)
{
	if(nslot<0)
	{
		for(int x=0;x<numSlots;x++)
			slots[x].desconectar();
		sockServer.disconnect();
		sockSel.clear();
	}
	else if(nslot<numSlots)
	{
		sockSel.remove(slots[nslot].getFD());
		slots[nslot].desconectar();
	}
	else
		return -1;
    return 0;
}

int MBSSlotManager::select()
{
	int rc=sockSel.Select();
	if(rc<0)
	{
		cout << "sel error (not handled)=" << rc << endl;
		return -1;
	}

	if(sockSel.isRead(sockServer.fd))
	{
		int s=reserve();
		if(s<0)
		{
			cout << "Server is full" << endl;
			sockServer.refuse();
		}
		else
		{
			slots[s].conectar(sockServer.aceitar());
			sockSel.add(slots[s].getFD());
		}
	}

	if(sockSel.isException(sockServer.fd))
	{
		cout << "Exception socket server" << endl;
		sockSel.remove(sockServer.fd);
		sockServer.disconnect();
	}

	return rc;
}
