#include "ProtocoloP2P.h"
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

int Slot::conectar(Conexao *con)
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

int Slot::conectar(const char *ip, const unsigned short porta)
{
	if(c==NULL)
		c=new Conexao();
	c->pai=this;
	c->registraCallback(tratar);
	timestamp=time(NULL);
	return c->conectar(ip,porta);
}

int Slot::conectar(const Noh& n)
{
	if(c==NULL)
		c=new Conexao();
	c->pai=this;
	c->registraCallback(tratar);
	timestamp=time(NULL);
	return c->conectar(n.ip,n.porta);
}

int Slot::desconectar()
{
	m.trava();
	_reset();
	m.destrava();
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
    if(estado>RESERVADO)
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
	while(!recebidos.empty())
	{
		delete recebidos.front();
		recebidos.pop();
	}
    return 0;
}

bool Slot::_conectado()
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
	COMANDO comando=frame->readShort();
	switch(comando)
    {
    	case DIRETA:
			return ph->IPH_tratar(frame,slot);	//repassa pra camada superior
		case PING:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PING");
			#endif
			unsigned long timestamp=frame->readLong();
			Buffer *pong=new Buffer(sizeof(COMANDO)+sizeof(timestamp));
			pong->writeShort((COMANDO)PONG);
			pong->writeLong(timestamp);
			slot->enviar(pong);
		break;
		case PONG:
			#ifdef LOGAR_COMANDOS
				logar("CMD_PONG");
			#endif
			unsigned long base=frame->readLong();
			unsigned long agora=(clock()/(CLOCKS_PER_SEC/1000));
			slot->iC.ping=agora-base;
		break;
		default:
			logar("CAMADA1_COMANDO_INVALIDO:");
		break;
    }
    delete frame;
	return 0;
}

int GerenciadorSlots::IFH_conectado(Slot *slot)
{
	nohs.push(slot->iC);
	ph->IPH_conectado(slot->iC);
	return 0;
}

int GerenciadorSlots::IFH_desconectado(Slot *slot)
{
	ph->IPH_desconectado(slot->iC);
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

Slot* GerenciadorSlots::operator[](const Noh& n) const
{
    for(int x=0;x<numSlots;x++)
    	if(n==slots[x].iC)
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
            if(slots[x].pegaEstado()==Slot::LIVRE)
            {
                if(slots[x].setaEstado(Slot::RESERVADO)==0)
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

int GerenciadorSlots::enviar(Buffer *pkt, const Noh& n)
{
	Slot *s=operator[](n);
	if(s==NULL)
		return -1;

	Buffer *frame=new Buffer(sizeof(COMANDO)+pkt->pegaTamanho());
	frame->writeShort(DIRETA);
	frame->append(*pkt);
	delete pkt;

	return s->enviar(frame);
}

int GerenciadorSlots::ouvir(unsigned short porta)
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

int GerenciadorSlots::conectar(const Noh& n)
{
    int ns=aloca();
    if(ns<0)
        return ns;  //nenhum slot disponivel
	int ret=slots[ns].conectar(n);
	if(ret==0)
		slots[ns].setaEstado(Slot::LOGIN);
	else
		slots[ns].setaEstado(Slot::LIVRE);
	return ret;
}

int GerenciadorSlots::conectar(const char *ip, const unsigned short porta)
{
    int ns=aloca();
    if(ns<0)
        return ns;  //nenhum slot disponivel
	int ret=slots[ns].conectar(ip,porta);
	if(ret==0)
		slots[ns].setaEstado(Slot::LOGIN);
	else
		slots[ns].setaEstado(Slot::LIVRE);
	return ret;
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

void GerenciadorSlots::ping()
{
	Buffer *ping;
	unsigned long timestamp=(clock()/(CLOCKS_PER_SEC/1000));

	for(int x=0;x<numSlots;x++)
	{
		ping=new Buffer(sizeof(COMANDO)+sizeof(timestamp));
		ping->writeShort((COMANDO)PING);
		ping->writeLong(timestamp);
		slots[x].enviar(ping);
	}
}

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

int ClienteP2P::enviarMsg(string msg, const Hash128* user)
{
	return enviarMsg(msg.c_str(),user);
}

int ClienteP2P::enviarMsg(const char *msg, const Hash128* user)
//  -1 = desconectado
//  -2 = erro
{
//TODO: begin gambia
    Slot *s=slots[*user];
    if(s==NULL)
		return -1;
	Noh n=s->iC;
//TODO: end gambia

    unsigned short tam=strlen(msg);
    Buffer *p=new Buffer(sizeof(COMANDO)+tam);	//comando + dados
    if(p==NULL)
        return -2;
    p->writeShort((COMANDO)MENSAGEM);
    memcpy(p->pntE,msg,tam);
    p->pntE+=tam;

    if(slots.enviar(p,n))
        return -2;
    return 0;
}

int ClienteP2P::enviaLogin(const Noh& n)
{
	Buffer *login=new Buffer(56);
	login->writeShort((COMANDO)LOGIN);
	slots.iC.write(login->pntE);
	iU.write(login->pntE);
	if(slots.enviar(login,n))
		return -1;
	return 0;
}

int ClienteP2P::IPH_tratar(Buffer *pacote, Slot *slot)
{
	COMANDO comando=pacote->readShort();
    switch(comando)
    {
		case LOGIN:
			#ifdef LOGAR_COMANDOS
				logar("CMD_LOGIN");
			#endif
			slot->iC.read(pacote->pntL);
			slot->iU.read(pacote->pntL);
			slot->setaEstado(Slot::CONECTADO);
			Usuario *tmp=usuarios.busca(slot->iU);
			if(tmp==NULL)
				usuarios.insere(slot->iU,new Usuario(slot->iU));
		break;
		case MENSAGEM:
			#ifdef LOGAR_COMANDOS
				logar("CMD_MENSAGEMDIRETA");
			#endif
			mostrarMsg(&slot->iU,string((char*)pacote->pntL,(size_t)pacote->disponiveis()));
		break;
		default:
			logar("CMD_INVALIDO");
		break;
    }
    delete pacote;
    return 0;
}

int ClienteP2P::IPH_conectado(const Noh& n)
{
	enviaLogin(n);
	return 0;
}

int ClienteP2P::IPH_desconectado(const Noh& n)
{
	return 0;
}

