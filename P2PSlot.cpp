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

#ifdef _WIN32

#include "P2PSlot.h"
#include "P2Pdefs.h"

namespace
{
//	const int TAMBUFFER=500;	//Buffer temp
	const int TIMEOUT_RX=60;	//timeout de recepcao de frames (em segundos)
};

using namespace std;
using namespace Protocolo;

void logar(string frase);

//------------------------------------------------------------------------------
//      P2PSlot
//------------------------------------------------------------------------------

int P2PSlot::conectar(Conexao *con)
{
	if(estado!=RESERVADO)
	{
		return -1;
	}
	if(con==NULL)
	{
		estado=LIVRE;
		return -1;
	}
	c=con;
	c->pai=this;
	c->registraCallback(tratar);
	timestamp=time(NULL);
	return 0;
}

int P2PSlot::conectar(const char *ip, const unsigned short porta)
{
	estado=RESERVADO;
	if(c==NULL)
		c=new Conexao();
	c->pai=this;
	c->registraCallback(tratar);
	timestamp=time(NULL);
	return c->conectar(ip,porta);
}

int P2PSlot::conectar(const Noh& n)
{
	if(c==NULL)
		c=new Conexao();
	c->pai=this;
	c->registraCallback(tratar);
	timestamp=time(NULL);
	return c->conectar(n.ip,n.porta);
}

int P2PSlot::desconectar()
{
	cs.trava();
	_reset();
	cs.destrava();
    return 0;
}

P2PSlot::EstadoSlot P2PSlot::pegaEstado()
{
    cs.trava();
   	_conectado();
    cs.destrava();
    return estado;
}

int P2PSlot::setaEstado(EstadoSlot novo)
{
    cs.trava();
	estado=novo;
    cs.destrava();
    return 0;
}

int P2PSlot::enviar(Buffer *pkt)
{
	int retorno=-1;
	cs.trava();
	_conectado();
    if(estado>RESERVADO)
    {
        TAMANHO tam=pkt->pegaTamanho();
        if(c->enviar((char*)&tam,sizeof(TAMANHO))==sizeof(TAMANHO)) //envia tamanho
            if(c->enviar((char*)pkt->dados,tam)==tam)       		//envia pacote
                retorno=0;                                  		//tudo ok
    }
//    delete pkt;
    cs.destrava();
    return retorno;
}

Buffer* P2PSlot::receber()
{
	int qtd;
	cs.trava();
	do
	{
        temp.reset();
		qtd=c->receber((char*)temp.pntE, temp.livres());
//		cout << "qtd=" << qtd << endl;
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
					logar("__FILE__:__LINE__:IMPOSSIVEL");
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
	cs.destrava();
	return retorno;
}

int P2PSlot::_reset()
{
	estado=LIVRE;
	estadoRX=NOVO;
	if(c!=NULL)
	{
        delete c;
		c=NULL;
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
    return 0;
}

bool P2PSlot::_conectado()
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

int P2PSlot::tratar(Conexao *con, long codeve, long coderro[])
{
	if(con==NULL)
		return -1;

	P2PSlot *s=(P2PSlot*)con->pai;
	if(s==NULL)
		return -1;

	s->iC.ip=con->pegaInfo()->sin_addr.s_addr;
	s->iC.porta=con->pegaInfo()->sin_port;

	if(codeve & FD_CONNECT)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_CONNECT");
		#endif
		if(coderro[FD_CONNECT_BIT]!=0)
			return 1;
	}
	if(codeve & FD_READ)
	{
		if(coderro[FD_READ_BIT]!=0)
		{
			#ifdef LOGAR_SOCKET
				logar("FD_READ ERRO");
			#endif
			return 1;
		}
		else
		{
			#ifdef LOGAR_SOCKET
				logar("FD_READ");
			#endif
			Buffer *f;
			if(s->gerenciador)
			{
                while((f=s->receber())!=NULL)
                {
                	#ifdef LOGAR_SOCKET
						logar("IFH_tratar");
					#endif
                    s->gerenciador->IFH_tratar(f,s);
                }
			}
			#ifdef LOGAR_SOCKET
			else
				logar("Pacote descartado");
			#endif
		}
	}
	if(codeve & FD_WRITE)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_WRITE");
		#endif
		if(coderro[FD_WRITE_BIT]!=0)
			return 1;
        if(s->gerenciador)
            s->gerenciador->IFH_conectado(s);
		else
			if(s->estado<HANDSHAKE)
				s->estado=HANDSHAKE;
	}
	if(codeve & FD_CLOSE)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_CLOSE");
		#endif
		if(s->gerenciador)
            s->gerenciador->IFH_desconectado(s);
		return 1;	//matar thread...
	}
	return 0;
}
#endif
