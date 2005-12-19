#include "ProtocoloP2P.h"
#include <ctime>
#include <ios>
#include <fstream>

//#define LOGAR_COMANDOS
//#define LOGAR_SOCKET

using namespace std;
using namespace Protocolo;

namespace
{
	const int TAMBUFFER=500;
	ClienteP2PUI *p2pui=NULL;
	Mutex mutexMostrarMsg;
	Mutex mutexLogar;
	void mostrarMsg(Hash128 *user, string frase)
	{
		mutexMostrarMsg.trava();
		if(p2pui!=NULL)
			p2pui->mostraMensagemChat(user,frase);
		else
		{
			cerr << "ClienteP2PUI::mostraMensagemChat(" << user->toString();
			cerr << ", " << frase << ")" << endl;
		}
		mutexMostrarMsg.destrava();
	}
	void logar(string frase)
	{
		mutexLogar.trava();
		if(p2pui!=NULL)
			p2pui->relatorio(frase);
		else
			cerr << "ClienteP2PUI::relatorio(" << frase << ")" << endl;
		mutexLogar.destrava();
	}
};

//------------------------------------------------------------------------------
//      Slot
//------------------------------------------------------------------------------

Slot::Slot()
{
    temp.mudaTamanho(TAMBUFFER);
    c=NULL;
    recebendo=NULL;
    _reset();
}

Slot::~Slot()
{
	_reset();
}

int Slot::desconectar()
{
	_reset();
    return 0;
}

Slot::EstadoSlot Slot::pegaEstado()
{
    m.trava();
   	_conectado();
    m.destrava();
    return estado;
}

int Slot::setaEstado(EstadoSlot novo)
{
    m.trava();
	estado=novo;
    m.destrava();
    return 0;
}

int Slot::enviar(Buffer *pkt)
{
	int retorno=-1;
	m.trava();
	_conectado();
    if(estado>ESPERANDO)
    {
        TAMANHO tam=pkt->pegaTamanho();
        if(c->enviar((char*)&tam,sizeof(TAMANHO))==sizeof(TAMANHO)) //envia tamanho
            if(c->enviar((char*)pkt->dados,tam)==tam)       		//envia pacote
                retorno=0;                                  		//tudo ok
    }
    delete pkt;
    m.destrava();
    return retorno;
}

Buffer* Slot::receber()
{
	int qtd;
	m.trava();
	do
	{
        temp.reset();
		qtd=c->receber((char*)temp.pntE, temp.livres());
		if(qtd>0)
			temp.pntE+=qtd;
		while(temp.disponiveis()>0)
		{
			switch(estadoRX)
			{
				case NOVO:
					//esperando primeiro byte de tam do novo pacote
					*((char*)&tamRecebendo)=*temp.pntL;
					temp.pntL++;
					estadoRX=ESPERA_TAMANHO;
				break;
				case ESPERA_TAMANHO:
					//esperando segundo byte de tamanho
					*((char*)(&tamRecebendo+1))=*temp.pntL;
					temp.pntL++;
					//tamanho recebido, agora o pacote em si
					recebendo=new Buffer(tamRecebendo);
					estadoRX=DADOS;
				break;
				case DADOS:
					//receber comando e dados
					recebendo->append(temp,recebendo->livres());
					if(recebendo->livres()==0)	//aceitacao
					{
						#ifdef LOGAR_SOCKET
							logar("pacote completo recebido");
						#endif
						recebidos.push(recebendo);
						recebendo=NULL;
						estadoRX=NOVO;
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
	m.destrava();
	return retorno;
}

int Slot::_reset()
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
//TODO:    recebidos.clear();
    return 0;
}

bool Slot::_conectado()
{
	if(c!=NULL)
	{
		if(!c->ativa())
			_reset();
		else
			return true;
	}
	else
		_reset();
	return false;
}

int Slot::tratar(Conexao *con, long codeve, long coderro[])
{
	if(con==NULL)
		return -1;

	Slot *s=(Slot*)con->pai;
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
		#ifdef LOGAR_SOCKET
			logar("FD_READ");
		#endif
		if(coderro[FD_READ_BIT]!=0)
			return 1;
		else
		{
			Buffer *f;
			while((f=s->receber())!=NULL)
				s->gerenciador->IFH_tratar(f,s);
		}
	}
	if(codeve & FD_WRITE)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_WRITE");
		#endif
		if(coderro[FD_WRITE_BIT]!=0)
			return 1;
		s->gerenciador->IFH_conectado(s);
	}
	if(codeve & FD_CLOSE)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_CLOSE");
		#endif
		return 1;	//matar thread...
	}
	return 0;
}

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
	COMANDO comando=*((COMANDO*)frame->dados);
	unsigned char *dados=frame->dados+sizeof(COMANDO);
	unsigned long tamDados=frame->pegaTamanho()-sizeof(COMANDO);
	//TODO: remover comando
	switch(comando)
    {
/*
		case LOGIN:
			#ifdef LOGAR_COMANDOS
				logar("CMD_LOGIN");
			#endif
			slot->iC.read(dados);
			slot->iU.read(dados);
			slot->setaEstado(Slot::CONECTADO);
			Usuario *tmp=usuarios.busca(slot->iU);
			if(tmp==NULL)
				usuarios.insere(slot->iU,new Usuario(slot->iU));
			//TODO: mandar informacoes sobre o cliente (i.e. ID)
		break;
		case MENSAGEM:
			#ifdef LOGAR_COMANDOS
				logar("CMD_MENSAGEMDIRETA");
			#endif
			mostrarMsg(&slot->iU,string((char*)dados,(size_t)tamDados));
		break;
		case PING:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PING");
			#endif
			unsigned long timestamp=*((unsigned long*)dados);
			enviaPong(timestamp, slot);
		break;
		case PONG:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PONG");
			#endif
			unsigned long base=*((unsigned long*)dados);
			unsigned long agora=(clock()/(CLOCKS_PER_SEC/1000));
			slot->iC.ping=agora-base;
		break;
    }
*/
		default:
//			logar("CAMADA1_COMANDO_INVALIDO:");
		break;
    }
	ph->IPH_tratar(frame,slot);
	return 0;
}

int GerenciadorSlots::IFH_conectado(Slot *slot)
{
	nohs.push(slot->iC);
	ph->IPH_conectado(slot);
	return 0;
}

int GerenciadorSlots::IFH_desconectado(Slot *slot)
{
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

Slot* GerenciadorSlots::operator[](const Hash128& user) const
{
    for(int x=0;x<numSlots;x++)
    	if(slots[x].iU.cmp(user)==0)
			return &slots[x];
    return NULL;
}

int GerenciadorSlots::aloca()
// busca e aloca (estado 0->1) slot livre
// retorna num do slot ou -1 caso todos estejam ocupados
{
    m.trava();
    {
        for(int x=0;x<numSlots;x++)
        {
            if(slots[x].pegaEstado()==0)
            {
                if(slots[x].setaEstado(Slot::LOGIN)==0)
                {
                    m.destrava();
                    return x;
                }
            }
        }
    }
    m.destrava();
    return -1;
}

int GerenciadorSlots::conectar(const char *ip, const unsigned short porta)
{
    Slot *s;
    int ns=aloca();
    if(ns<0)
        return ns;  //nenhum slot disponivel
    s=&slots[ns];

    if(s->c==NULL)
        s->c=new Conexao();
    Conexao *c=s->c;
    c->pai=s;
    c->id=ns;
    c->registraCallback(Slot::tratar);
    return c->conectar(ip,porta);
}

int GerenciadorSlots::conectar(std::string ip, const unsigned short porta)
{
	return conectar(ip.c_str(),porta);
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

int GerenciadorSlots::tratarServer(Conexao *con, long codeve, long coderro[])
{
    GerenciadorSlots *pai;
    int ns;
    Slot *s;

    if((pai=(GerenciadorSlots*)con->pai)==NULL)
        return -1;

	if(codeve & FD_ACCEPT)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_ACCEPT");
		#endif
        ns=pai->aloca();
        if(ns<0)
        {
            con->recusar();
            logar("Conexão recusada");
            return 0;
        }
        s=&pai->slots[ns];
		s->_reset();	//TODO: _reset() naum eh publico
        s->c=con->aceitar();
        if(s->c!=NULL)
        {
            s->c->pai=s;
            s->c->id=ns;
            s->c->registraCallback(Slot::tratar);
        }
	}
	if(codeve & FD_CLOSE)
		return 1;	//matar thread...
	return 0;
}

//------------------------------------------------------------------------------
//      Cliente
//------------------------------------------------------------------------------

ClienteP2P::ClienteP2P()
{
    strcpy(iU.nick,"Noob");
	iU.Hash128::random();
	slots.registraPacketHandler(this);
}

ClienteP2P::~ClienteP2P()
{
}

void ClienteP2P::registraUI(ClienteP2PUI *ui)
{
	p2pui=ui;
}

int ClienteP2P::abrir(std::string dir)
{
	ifstream arq;
	IOArquivo ret;
	string arquivo;

	arquivo="id.dat";
	ret=abrirID(arquivo);
	if(ERRO_VERSAO==ret)
		logar("Versao incorreta do arquivo "+arquivo+"\n");
	else if(ERRO_IO==ret)
		logar("Erro ao ler o arquivo "+arquivo+"\n");

//TODO:	iC.read(arq);

	arquivo="usuarios.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::in);
	if(arq.good())
		usuarios.read(arq);
	else
		logar("Erro ao ler o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

	arquivo="clientes.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::in);
	if(arq.good())
		slots.nohs.read(arq);
	else
		logar("Erro ao ler o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

	return 0;
}

int ClienteP2P::salvar(std::string dir)
{
	ofstream arq;
	IOArquivo ret;
	string arquivo;

	arquivo="id.dat";
	ret=salvarID(arquivo);
	if(ERRO_IO==ret)
		logar("Erro ao gravar o arquivo "+arquivo+"\n");

//TODO:	iC.write(arq);

	arquivo="usuarios.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::out|ios::trunc);
	if(arq.good())
		usuarios.write(arq);
	else
		logar("Erro ao gravar o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

    arquivo="clientes.dat";
	arq.open(arquivo.c_str(),ios::binary|ios::out|ios::trunc);
	if(arq.good())
		slots.nohs.write(arq);
	else
		logar("Erro ao gravar o arquivo "+arquivo+"\n");
    arq.close();
    arq.clear();

	return 0;
}

IOArquivo ClienteP2P::abrirID(std::string arquivo)
{
	ifstream arq;

	arq.open(arquivo.c_str(),ios::binary|ios::in);
	if(arq.good())
	{
		int ver;
		if(arq.read((char*)&ver,sizeof(ver)))
		{
			if(VERSAOINFO==ver)
			{
				if(iU.read(arq))
				{
					amigos.limpa();
					if(amigos.read(arq))
						return OK;
				}
			}
			else
				return ERRO_VERSAO;
		}
	}
	return ERRO_IO;
}

IOArquivo ClienteP2P::salvarID(std::string arquivo)
{
	ofstream arq;

	arq.open(arquivo.c_str(),ios::binary|ios::out|ios::trunc);
	if(arq.good())
	{
		int v=VERSAOINFO;
		if(arq.write((char*)&v,sizeof(v)))
			if(iU.write(arq))
				if(amigos.write(arq))
					return OK;
	}
	return ERRO_IO;
}

int ClienteP2P::esperarConexoes(unsigned short porta)
{
	int retorno=0;
	slots.serverSock.desconectar();
	if(0==porta)
		porta=iC.porta;
	if((retorno=slots.serverSock.ouvir(porta))==0)
	{
		logar("Esperando conexoes na porta "+int2str(porta));
		iC.porta=porta;
	}
	else
		logar("Erro ao tentar abrir porta "+int2str(porta));
	return retorno;
}

int ClienteP2P::enviarMsg(string msg, const Hash128* user)
{
	return enviarMsg(msg.c_str(),user);
}

int ClienteP2P::enviarMsg(const char *msg, const Hash128* user)
//  -1 = desconectado
//  -2 = erro
{
    Slot *s=slots[*user];
    if(s==NULL)
		return -1;

    unsigned short tam=strlen(msg);
    Buffer *p=new Buffer(sizeof(COMANDO)+tam);	//comando + dados
    if(p==NULL)
        return -2;
    p->writeShort((COMANDO)MENSAGEM);
    memcpy(p->pntE,msg,tam);
    if(s->enviar(p))
        return -2;
    return 0;
}

void ClienteP2P::ping()
{
	for(int x=0;x<slots.pegaNumSlots();x++)
		enviaPing(slots[x]);
}

int ClienteP2P::enviaLogin(Slot *slot)
{
	Buffer *login=new Buffer(56);
	login->writeShort((COMANDO)LOGIN);
	iC.write(login->pntE);
	iU.write(login->pntE);
	if(slot->enviar(login))
		return -1;
	return 0;
}

int ClienteP2P::enviaPing(Slot *slot)
{
	Buffer *ping=new Buffer(6);
	ping->writeShort((COMANDO)PING);
	unsigned long timestamp=(clock()/(CLOCKS_PER_SEC/1000));
	ping->writeLong(timestamp);
	if(slot->enviar(ping))
		return -1;
	return 0;
}

int ClienteP2P::enviaPong(unsigned long timestamp, Slot *slot)
{
	Buffer *pong=new Buffer(6);
	pong->writeShort((COMANDO)PONG);
	pong->writeLong(timestamp);
	if(slot->enviar(pong))
		return -1;
	return 0;
}

int ClienteP2P::IPH_tratar(Buffer *pacote, Slot *slot)
{
	COMANDO comando=*((COMANDO*)pacote->dados);
    unsigned char *dados=pacote->dados+sizeof(COMANDO);
    unsigned long tamDados=pacote->pegaTamanho()-sizeof(COMANDO);

    switch(comando)
    {
		case LOGIN:
			#ifdef LOGAR_COMANDOS
				logar("CMD_LOGIN");
			#endif
			slot->iC.read(dados);
			slot->iU.read(dados);
			slot->setaEstado(Slot::CONECTADO);
			Usuario *tmp=usuarios.busca(slot->iU);
			if(tmp==NULL)
				usuarios.insere(slot->iU,new Usuario(slot->iU));
			//TODO: mandar informacoes sobre o cliente (i.e. ID)
		break;
		case MENSAGEM:
			#ifdef LOGAR_COMANDOS
				logar("CMD_MENSAGEMDIRETA");
			#endif
			mostrarMsg(&slot->iU,string((char*)dados,(size_t)tamDados));
		break;
		case PING:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PING");
			#endif
			unsigned long timestamp=*((unsigned long*)dados);
			enviaPong(timestamp, slot);
		break;
		case PONG:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PONG");
			#endif
			unsigned long base=*((unsigned long*)dados);
			unsigned long agora=(clock()/(CLOCKS_PER_SEC/1000));
			slot->iC.ping=agora-base;
		break;
		default:
			logar("CMD_INVALIDO");
		break;
    }
    delete pacote;
    return 0;
}

int ClienteP2P::IPH_conectado(Slot *slot)
{
	enviaLogin(slot);
	return 0;
}

int ClienteP2P::IPH_desconectado(Slot *slot)
{
	return 0;
}

