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

#include "P2PGerenciadorSlots.h"
#include "Parser.h"

namespace
{
	typedef enum CmdCamada1
	{
		DIRETA,
		/*
			[dados]
		*/
		PING,
		/*	Ping
			[unsigned long timestamp]
		*/
		PONG,
		/*	Resposta ao ping
			[unsigned long timestamp] (copia do recebido em ping)
		*/
		INFO_CLIENTE,
		/*	Informacoes sobre o cliente, recursos disponiveis no protocolo.
			Usado no processo de hand-shake.
			[Cliente]
				infoCliente = [versao][opcoes][Noh][mtu][mru]
					Noh = [ip][porta][id]
						sizeof(Noh)=10
					sizeof(infoCliente)=12+sizeof(Noh)=22
		*/
		ID_ASK,
		/*	Pede conexao callback pra teste de porta de entrada
			[]
		*/
		ID_RET,
		/*	Resposta de ID_ASK, se !idAlta() pedido de roteamento foi aceito
			[Noh]
		*/
		ID_ND,
		/*	Resposta de ID_ASK, !idAlta() & pedido de roteamento recusado
			[]
		*/
		NOH_LIST_ASK,
		/*	Pedido de lista de Nohs idAlta conhecidos
			[]
		*/
		NOH_LIST,
		/*	Lista de Nohs
			[unsigned char n][n*[Noh]]
		*/
		ROTEAR,
		/*	Pedido de roteamento do pacote pra cliente com id baixa
			[Noh destinatario][Noh remetente][dados]
		*/
		ROTEAR_ERRO,
		/*	Nao foi possivel rotear pacote (destinatario desconhecido)
			[Noh destinatario]
		*/
		BROADCAST,
		/*
			[Noh remetente][unsigned long seqno][unsigned char TTL][dados]
		*/
		COMPACTADO,
		/*
			[unsigned char METODO][dados]
		*/
		DROP
	};
};

/*	Processo de conexao

	C - cliente, quem inicia a conexao
	S - servidor, recebe a conexao

	Cliente							|	Servidor
	-------------------------------------------------------------------------
	envia(INFO_CLIENTE)					envia(INFO_CLIENTE)
	recebe(INFO_CLIENTE)				recebe(INFO_CLIENTE)
	se !inicializado
		envia(ID_ASK)
										recebe(ID_ASK)
										se conecta(C)
											envia(ID_RET Noh idAlta)
										senao se(recursos_de roteador_disponiveis)
											envia(ID_RET Noh idBaixa)
										senao
											envia(ID_ND)
	se recebe(ID_RET)
		inicializado=true
	senao
		envia(NOH_LIST_ASK)
										recebe(NOH_LIST_ASK)
										envia(NOH_LIST)
*/

using namespace std;
using namespace Protocolo;

void logar(string frase);

//------------------------------------------------------------------------------
//      P2PGerenciadorSlots
//------------------------------------------------------------------------------

P2PGerenciadorSlots::P2PGerenciadorSlots(int num)
{
	slots=NULL;
	numSlots=0;
	mudaNumSlots(num);
    serverSock.pai=this;
    serverSock.registraCallback(tratarServer);
}

P2PGerenciadorSlots::~P2PGerenciadorSlots()
{
    if((slots!=NULL)&&(numSlots>0))
        delete[] slots;
}

int P2PGerenciadorSlots::IFH_tratar(Buffer *frame, P2PSlot *slot)
{
	unsigned char tmp;
	frame->readByte(&tmp);
	COMANDO comando=tmp;
	switch(comando)
    {
    	case DIRETA:
			return ph->IPH_tratar(frame,slot->iC);	//repassa pra camada superior
		case PING:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PING");
			#endif
			{
				unsigned long timestamp;
				frame->readLong(&timestamp);
				Buffer *pong=new Buffer(sizeof(COMANDO)+sizeof(timestamp));
				pong->writeByte((COMANDO)PONG);
				pong->writeLong(timestamp);
				slot->enviar(pong);
			}
		break;
		case PONG:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PONG");
			#endif
			{
				unsigned long base;
				frame->readLong(&base);
				unsigned long agora=(clock()/(CLOCKS_PER_SEC/1000));
				slot->iC.ping=agora-base;
			}
		break;
		case ID_ASK:
			#ifdef LOGAR_COMANDOS
				logar("CMD_ID_ASK");
			#endif
			{
				Buffer *id_ask=new Buffer(sizeof(COMANDO)+sizeof(Noh));
				if(idAlta(slot->iC))
					id_ask->writeByte((COMANDO)ID_RET);
				else
					id_ask->writeByte((COMANDO)ID_ND);
				slot->enviar(id_ask);
			}
		break;
		case ID_RET:
			#ifdef LOGAR_COMANDOS
				logar("CMD_ID_RET");
			#endif
		break;
		case ID_ND:
			#ifdef LOGAR_COMANDOS
				logar("CMD_ID_ND");
			#endif
		break;
		default:
			#ifdef LOGAR_COMANDOS
				logar("CAMADA1_COMANDO_INVALIDO:");
			#endif
		break;
    }
    delete frame;
	return 0;
}

int P2PGerenciadorSlots::IFH_conectado(P2PSlot *slot)
{
	Buffer *id_ask=new Buffer(sizeof(COMANDO));//+sizeof(Noh));
	id_ask->writeByte((COMANDO)ID_ASK);
	slot->enviar(id_ask);

	nohs.push(slot->iC);
	ph->IPH_conectado(slot->iC);
	return 0;
}

int P2PGerenciadorSlots::IFH_desconectado(P2PSlot *slot)
{
	ph->IPH_desconectado(slot->iC);
	return 0;
}

int P2PGerenciadorSlots::mudaNumSlots(int num)
{
	if(num>0)
	{
		P2PSlot *temp=new P2PSlot[num];
		if(temp==NULL)
			return -1;		//erro de alocacao
		if((slots!=NULL)&&(numSlots>0))
			delete[] slots;
		slots=temp;
		for(int n=0;n<num;n++)
			slots[n].registraFrameHandler(this);
	}
	else if((slots!=NULL)&&(numSlots>0))
	{
		delete[] slots;
		slots=NULL;
	}
	numSlots=num;
	return 0;
}

P2PSlot* P2PGerenciadorSlots::at(int num) const
{
    if((num>=0)&(num<numSlots))
        return &slots[num];
    return NULL;
}

P2PSlot* P2PGerenciadorSlots::operator[](const Noh& n) const
{
    for(int x=0;x<numSlots;x++)
    	if(n==slots[x].iC)
			return &slots[x];
    return NULL;
}

int P2PGerenciadorSlots::aloca()
// busca e aloca (estado 0->1) slot livre
// retorna num do slot ou -1 caso todos estejam ocupados
{
    cs.trava();
    {
        for(int x=0;x<numSlots;x++)
        {
            if(slots[x].pegaEstado()==P2PSlot::LIVRE)
            {
                if(slots[x].setaEstado(P2PSlot::RESERVADO)==0)
                {
                    cs.destrava();
                    return x;
                }
            }
        }
    }
    cs.destrava();
    return -1;
}

int P2PGerenciadorSlots::enviar(Buffer *pkt, const Noh& n)
{
	P2PSlot *s=operator[](n);
	if(s==NULL)
		return -1;

	Buffer *frame=new Buffer(sizeof(COMANDO)+pkt->pegaTamanho());
	frame->writeByte(DIRETA);
	frame->append(*pkt);
	delete pkt;

	return s->enviar(frame);
}

int P2PGerenciadorSlots::ouvir(unsigned short porta)
{
	int retorno=0;
	serverSock.desconectar();
	if(0==porta)
		porta=iC.porta;
	if((retorno=serverSock.ouvir(porta))==0)
	{
		logar("Esperando conexoes na porta "+int2str(porta));
		iC.porta=porta;
	}
	else
		logar("Erro ao tentar abrir porta "+int2str(porta));
	return retorno;
}

int P2PGerenciadorSlots::conectar(const Noh& n)
{
    int ns=aloca();
    if(ns<0)
        return ns;  //nenhum slot disponivel
	int ret=slots[ns].conectar(n);
	if(ret==0)
		slots[ns].setaEstado(P2PSlot::LOGIN);
	else
		slots[ns].setaEstado(P2PSlot::LIVRE);
	return ret;
}

int P2PGerenciadorSlots::conectar(const char *ip, const unsigned short porta)
{
    int ns=aloca();
    if(ns<0)
        return ns;  //nenhum slot disponivel
	int ret=slots[ns].conectar(ip,porta);
	if(ret==0)
		slots[ns].setaEstado(P2PSlot::LOGIN);
	else
		slots[ns].setaEstado(P2PSlot::LIVRE);
	return ret;
}

int P2PGerenciadorSlots::conectar(std::string ip, const unsigned short porta)
{
	return conectar(ip.c_str(),porta);
}

int P2PGerenciadorSlots::desconectar(int nslot)
{
	if(nslot<0)
		for(int x=0;x<numSlots;x++)
			slots[x].desconectar();
	else if(nslot<numSlots)
		slots[nslot].desconectar();
	else
		return -1;
    return 0;
}

void P2PGerenciadorSlots::ping()
{
	Buffer *ping;
	unsigned long timestamp=(clock()/(CLOCKS_PER_SEC/1000));

	for(int x=0;x<numSlots;x++)
	{
		ping=new Buffer(sizeof(COMANDO)+sizeof(timestamp));
		ping->writeByte((COMANDO)PING);
		ping->writeLong(timestamp);
		slots[x].enviar(ping);
	}
}

int P2PGerenciadorSlots::tratarServer(Conexao *con, long codeve, long coderro[])
{
	P2PGerenciadorSlots *pai;
	if((pai=(P2PGerenciadorSlots*)con->pai)==NULL)
		return -1;

	if(codeve & FD_ACCEPT)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_ACCEPT");
		#endif
		int ns=pai->aloca();
		if(ns<0)
		{
			con->recusar();
			logar("Conexão recusada");
			return 0;
		}
		pai->slots[ns].conectar(con->aceitar());
	}
	if(codeve & FD_CLOSE)
		return 1;	//matar thread...
	return 0;
}

bool P2PGerenciadorSlots::idAlta(const Noh &n)
{
	Conexao ct;
	if(ct.conectar(n.ip,n.porta)==0)
		return true;
	return false;
}
