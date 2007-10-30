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

#include "GerenciadorSlots.h"
#include "Parser.h"

namespace
{
    typedef unsigned char COMANDO;
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

void logar(string frase);

//------------------------------------------------------------------------------
//      GerenciadorSlots
//------------------------------------------------------------------------------

GerenciadorSlots::GerenciadorSlots(int num)
{
	slots=NULL;
	numSlots=0;
	mudaNumSlots(num);
    serverSock.pai=this;
    serverSock.registraCallback(tratarServer);
}

GerenciadorSlots::~GerenciadorSlots()
{
    if((slots!=NULL)&&(numSlots>0))
        delete[] slots;
}

int GerenciadorSlots::IFH_tratar(Buffer *frame, Slot *slot)
{
    /*
	COMANDO comando=frame->readByte();
	switch(comando)
    {
    	case DIRETA:
    */
			return ph->IPH_tratar(frame,slot->id);	//repassa pra camada superior
    /*
		case PING:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PING");
			#endif
			{
				unsigned long timestamp=frame->readLong();
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
				unsigned long base=frame->readLong();
				unsigned long agora=(clock()/(CLOCKS_PER_SEC/1000));
				slot->ping=agora-base;
			}
		break;
		case ID_ASK:
		case ID_RET:
		case ID_ND:
		default:
			#ifdef LOGAR_COMANDOS
				logar("CAMADA1_COMANDO_INVALIDO:");
			#endif
		break;
    }
    */
    delete frame;
	return 0;
}

int GerenciadorSlots::IFH_conectado(Slot *slot)
{
/*
	Buffer *id_ask=new Buffer(sizeof(COMANDO));//+sizeof(Noh));
	id_ask->writeByte((COMANDO)ID_ASK);
	slot->enviar(id_ask);
*/
	ph->IPH_conectado(slot->id);
	return 0;
}

int GerenciadorSlots::IFH_desconectado(Slot *slot)
{
	ph->IPH_desconectado(slot->id);
	return 0;
}

int GerenciadorSlots::mudaNumSlots(int num)
{
	if(num>0)
	{
		Slot *temp=new Slot[num];
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

Slot* GerenciadorSlots::at(int num) const
{
    if((num>=0)&(num<numSlots))
        return &slots[num];
    return NULL;
}

Slot* GerenciadorSlots::operator[](std::string id) const
{
    for(int x=0;x<numSlots;x++)
    	if(id==slots[x].id)
			return &slots[x];
    return NULL;
}

int GerenciadorSlots::aloca()
// busca e aloca (estado 0->1) slot livre
// retorna num do slot ou -1 caso todos estejam ocupados
{
    cs.trava();
    {
        for(int x=0;x<numSlots;x++)
        {
            if(slots[x].pegaEstado()==Slot::LIVRE)
            {
                if(slots[x].setaEstado(Slot::RESERVADO)==0)
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

int GerenciadorSlots::enviar(Buffer *pkt, std::string id)
{
	Slot *s=operator[](id);
	if(s==NULL)
		return -1;

	Buffer *frame=new Buffer(sizeof(COMANDO)+pkt->pegaTamanho());
	frame->writeByte(DIRETA);
	frame->append(*pkt);
	delete pkt;

	return s->enviar(frame);
}

int GerenciadorSlots::ouvir(unsigned short porta)
{
	int retorno=0;
	serverSock.desconectar();
	if((retorno=serverSock.ouvir(porta))==0)
		logar("Esperando conexoes na porta "+int2str(porta));
	else
		logar("Erro ao tentar abrir porta "+int2str(porta));
	return retorno;
}

int GerenciadorSlots::desconectar(int nslot)
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
/*
void GerenciadorSlots::ping()
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
*/
int GerenciadorSlots::tratarServer(Conexao *con, long codeve, long coderro[])
{
	GerenciadorSlots *pai;
	if((pai=(GerenciadorSlots*)con->pai)==NULL)
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
