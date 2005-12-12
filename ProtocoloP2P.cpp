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
//      Noh
//------------------------------------------------------------------------------

Noh::Noh()
{
    ip=id=0;
    porta=PORTA_PADRAO;
}

int Noh::read(unsigned char *&pkt)
{
    ip=*((unsigned long*)pkt);
    pkt+=sizeof(ip);
    porta=*((unsigned short*)pkt);
    pkt+=sizeof(porta);
    id=*((unsigned long*)pkt);
    pkt+=sizeof(id);
    return 0;
}

int Noh::write(unsigned char *&pkt) const
{
    *((unsigned long*)pkt)=ip;
    pkt+=sizeof(ip);
    *((unsigned short*)pkt)=porta;
    pkt+=sizeof(porta);
    *((unsigned long*)pkt)=id;
    pkt+=sizeof(id);
    return 0;
}

std::istream& Noh::read(std::istream& is)
{
	is.read((char*)&ip,sizeof(ip));
	is.read((char*)&porta,sizeof(porta));
	is.read((char*)&id,sizeof(id));
	return is;
}

std::ostream& Noh::write(std::ostream& os) const
{
	os.write((char*)&ip,sizeof(ip));
	os.write((char*)&porta,sizeof(porta));
	os.write((char*)&id,sizeof(id));
	return os;
}

//------------------------------------------------------------------------------
//      Cliente
//------------------------------------------------------------------------------

Cliente::Cliente()
{
    ping=versao=opcoes=0;
    mtu=mru=2000;
}

int Cliente::read(unsigned char*& pkt)
{
    versao=*((unsigned long*)pkt);
    pkt+=sizeof(versao);
    opcoes=*((unsigned long*)pkt);
    pkt+=sizeof(opcoes);
    Noh::read(pkt);
    mtu=*((unsigned short*)pkt);
    pkt+=sizeof(mtu);
    mru=*((unsigned short*)pkt);
    pkt+=sizeof(mru);
    return 0;
}

int Cliente::write(unsigned char*& pkt) const
{
    *((unsigned long*)pkt)=versao;
    pkt+=sizeof(versao);
    *((unsigned long*)pkt)=opcoes;
    pkt+=sizeof(opcoes);
    Noh::write(pkt);
    *((unsigned short*)pkt)=mtu;
    pkt+=sizeof(mtu);
    *((unsigned short*)pkt)=mru;
    pkt+=sizeof(mru);
    return 0;
}

istream& Cliente::read(istream& is)
{
	is.read((char*)&versao,sizeof(versao));
	is.read((char*)&opcoes,sizeof(opcoes));
	Noh::read(is);
	return is;
}

ostream& Cliente::write(ostream& os)
{
	os.write((char*)&versao,sizeof(versao));
	os.write((char*)&opcoes,sizeof(opcoes));
	Noh::write(os);
	return os;
}

//------------------------------------------------------------------------------
//      Usuario
//------------------------------------------------------------------------------

Usuario::Usuario()
{
	nick[0]=0;
    rx=tx=0;
}

Usuario::Usuario(const Usuario& base)
{
    memcpy(nick,base.nick,TAMNICK);
    memcpy(h.b,base.h.b,16);
    rx=base.rx;
    tx=base.tx;
    codCli=base.codCli;
}

int Usuario::mudaNick(std::string novo)
{
	memset(nick,0,TAMNICK);
	novo.copy(nick,TAMNICK-1);
	return 0;
}

int Usuario::read(unsigned char*& pkt)
{
    Hash128::read(pkt);
    memcpy(nick,pkt,TAMNICK);
    pkt+=TAMNICK;
    return 0;
}

int Usuario::write(unsigned char *&pkt) const
{
    Hash128::write(pkt);
    memcpy(pkt,nick,TAMNICK);
    pkt+=TAMNICK;
    return 0;
}

istream& Usuario::read(istream& is)
{
	is.read((char*)h.b,sizeof(h));
	is.read(nick,TAMNICK);
	is.read((char*)&rx,sizeof(rx));
	is.read((char*)&tx,sizeof(tx));
	return is;
}

ostream& Usuario::write(ostream& os)
{
	os.write((char*)h.b,sizeof(h));
	os.write(nick,TAMNICK);
	os.write((char*)&rx,sizeof(rx));
	os.write((char*)&tx,sizeof(tx));
	return os;
}

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


//------------------------------------------------------------------------------
//      ArraySlots
//------------------------------------------------------------------------------

ArraySlots::ArraySlots(int num)
{
	slots=NULL;
	numSlots=0;
	mudaNumSlots(num);
}

ArraySlots::~ArraySlots()
{
    if((slots!=NULL)&&(numSlots>0))
        delete[] slots;
}

int ArraySlots::mudaNumSlots(int num)
{
	if(num>0)
	{
		Slot *temp=new Slot[num];
		if(temp==NULL)
			return -1;		//erro de alocacao
		if((slots!=NULL)&&(numSlots>0))
			delete[] slots;
		slots=temp;
	}
	else if((slots!=NULL)&&(numSlots>0))
	{
		delete[] slots;
		slots=NULL;
	}
	numSlots=num;
	return 0;
}

Slot* ArraySlots::at(int num) const
{
    if((num>=0)&(num<numSlots))
        return &slots[num];
    return NULL;
}

Slot* ArraySlots::operator[](const Hash128& user) const
{
    for(int x=0;x<numSlots;x++)
    	if(slots[x].iU.cmp(user)==0)
			return &slots[x];
    return NULL;
}

int ArraySlots::aloca()
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

int ArraySlots::conectar(const char *ip, const unsigned short porta)
{
    Slot *s;
    int ns=aloca();
    if(ns<0)
        return ns;  //nenhum slot disponivel
    s=&slots[ns];
    if(s->c==NULL)
        s->c=new Conexao();
    Conexao *c=s->c;
    c->pai=this;
    c->id=ns;
    c->registraCallback(ClienteP2P::tratar);
    return c->conectar(ip,porta);
}

int ArraySlots::conectar(std::string ip, const unsigned short porta)
{
	return conectar(ip.c_str(),porta);
}

int ArraySlots::desconectar(int nslot)
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

//------------------------------------------------------------------------------
//      ListaHash128
//------------------------------------------------------------------------------

int ListaHash128::insere(const Hash128& h)
{
	m.trava();
	if(binary_search(lista.begin(),lista.end(),h))
	{
		m.destrava();
		return -1;	//jah existe
	}
	set<Hash128>::iterator i=upper_bound(lista.begin(),lista.end(),h);
	lista.insert(i,h);
	m.destrava();
	return 0;
}

void ListaHash128::remove(const Hash128& h)
{
	m.trava();
	lista.erase(h);
	m.destrava();
}

bool ListaHash128::busca(const Hash128& h)
{
	m.trava();
	bool retorno=binary_search(lista.begin(),lista.end(),h);
	m.destrava();
	return retorno;
}

int ListaHash128::tamanho()
{
	m.trava();
	int tam=lista.size();
	m.destrava();
	return tam;
}

void ListaHash128::limpa()
{
	m.trava();
	lista.clear();
	m.destrava();
}

istream& ListaHash128::read(istream& is)
{
	Hash128 tmp;
	unsigned short numero;

	if(is.read((char*)&numero,sizeof(numero)))
		for(int n=0;n<numero;n++)
			if(is.read((char*)tmp.h.b,sizeof(tmp.h)))
				insere(tmp);
			else
				break;
	return is;
}

ostream& ListaHash128::write(ostream& os)
{
	m.trava();
	unsigned short numero=lista.size();
	if(os.write((char*)&numero,sizeof(numero)))
	{
		set<Hash128>::iterator i=lista.begin();
		while(i!=lista.end())
		{
			os.write((char*)(*i).h.b,sizeof((*i).h));
			i++;
		}
	}
	m.destrava();
	return os;
}

//------------------------------------------------------------------------------
//      ListaUsuarios
//------------------------------------------------------------------------------

bool ListaUsuarios::insere(const Hash128& h, Usuario* u)
{
	bool ret=true;
	m.trava();
	if(lista.find(h)!=lista.end())
		ret=false;	//jah existe
	else
		lista[h]=u;
	m.destrava();
	return ret;
}

void ListaUsuarios::remove(const Hash128& h)
{
	m.trava();
	lista.erase(h);
	m.destrava();
}

Usuario* ListaUsuarios::busca(const Hash128& h)
{
	Usuario* ret=NULL;
	m.trava();
	map<Hash128,Usuario*>::iterator i=lista.find(h);
	if(i!=lista.end())
		ret=i->second;
	m.destrava();
	return ret;
}

int ListaUsuarios::tamanho()
{
	m.trava();
	int tam=lista.size();
	m.destrava();
	return tam;
}

void ListaUsuarios::limpa()
{
	m.trava();
	lista.clear();
	m.destrava();
}

istream& ListaUsuarios::read(istream& is)
{
    Usuario tmp;
    while(tmp.read(is))
        lista[tmp]=new Usuario(tmp);
	return is;
}

ostream& ListaUsuarios::write(ostream& os)
{
    map<Hash128,Usuario*>::iterator i=lista.begin();
    while(i!=lista.end())
    {
        i->second->write(os);
        i++;
    }
	return os;
}

//------------------------------------------------------------------------------
//      ListaNohs
//------------------------------------------------------------------------------

void ListaNohs::push(const Noh& no)
{
	m.trava();
	lista.push_back(no);
	lista.unique();
	m.destrava();
}

Noh* ListaNohs::pop(const Noh& no)
{
	Noh *tmp;
	m.trava();
	if(lista.size()>0)
	{
		tmp=new Noh(lista.front());
		lista.pop_front();
	}
	else
		tmp=NULL;
	m.destrava();
	return tmp;
}

int ListaNohs::tamanho()
{
	m.trava();
	return lista.size();
	m.destrava();
}

void ListaNohs::limpa()
{
	m.trava();
	lista.clear();
	m.destrava();
}

istream& ListaNohs::read(istream& is)
{
	m.trava();
	int tam;
	Noh tmp;
	if(is.read((char*)&tam,sizeof(tam)))
		if(tam>0)
		{
			for(int n=0;n<tam;n++)
				if(tmp.read(is))
					lista.push_back(tmp);
				else
					break;
		}
	m.destrava();
	return is;
}

ostream& ListaNohs::write(ostream& os)
{
	m.trava();
	int tam=lista.size();
	os.write((char*)&tam,sizeof(tam));
	if(tam>0)
	{
		for(list<Noh>::iterator i=lista.begin();i!=lista.end();i++)
			if(!((*i).write(os)))
				break;
	}
	m.destrava();
	return os;
}

//------------------------------------------------------------------------------
//      Cliente
//------------------------------------------------------------------------------

ClienteP2P::ClienteP2P()
{
    strcpy(iU.nick,"Noob");
	iU.Hash128::random();
    serverSock.pai=this;
    serverSock.registraCallback(tratarServer);
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
		nohs.read(arq);
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
		nohs.write(arq);
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
    p->append((COMANDO)MENSAGEM);
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
	login->append((COMANDO)LOGIN);
	iC.write(login->pntE);
	iU.write(login->pntE);
	if(slot->enviar(login))
		return -1;
	return 0;
}

int ClienteP2P::enviaPing(Slot *slot)
{
	Buffer *ping=new Buffer(6);
	ping->append((COMANDO)PING);
//	*((COMANDO*)ping->dados)=PING;
//	unsigned char *dados=ping->dados+sizeof(COMANDO);
	unsigned long timestamp=(clock()/(CLOCKS_PER_SEC/1000));
//	*((unsigned long*)ping->pntE)=timestamp;
	ping->append(timestamp);
	if(slot->enviar(ping))
		return -1;
	return 0;
}

int ClienteP2P::enviaPong(unsigned long timestamp, Slot *slot)
{
	Buffer *pong=new Buffer(6);
	pong->append((COMANDO)PONG);
//	*((COMANDO*)pong->dados)=PONG;
//	unsigned char *dados=pong->dados+sizeof(COMANDO);
//	*((unsigned long*)pong->pntE)=timestamp;
	pong->append(timestamp);
	if(slot->enviar(pong))
		return -1;
	return 0;
}

int ClienteP2P::tratar(Buffer *pacote, Slot *slot)
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

int ClienteP2P::tratar(Conexao *con, long codeve, long coderro[])
{
	if(con==NULL)
		return -1;
	ClienteP2P *pai=(ClienteP2P*)con->pai;
	if(pai==NULL)
		return -1;

	Slot *s=pai->slots.at(con->id);
	if(s==NULL)
		return -1;

	s->iC.ip=con->pegaInfo()->sin_addr.s_addr;
	s->iC.porta=con->pegaInfo()->sin_port;

	if(codeve & FD_CONNECT)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_CONNECT");
		#endif
		pai->nohs.push(s->iC);
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
			Buffer *p;
			while((p=s->receber())!=NULL)
				pai->tratar(p,s);
		}
	}
	if(codeve & FD_WRITE)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_WRITE");
		#endif
		if(coderro[FD_WRITE_BIT]!=0)
			return 1;
		else
			pai->enviaLogin(s);
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

int ClienteP2P::tratarServer(Conexao *con, long codeve, long coderro[])
{
    ClienteP2P *pai;
    int ns;
    Slot *s;

    if((pai=(ClienteP2P*)con->pai)==NULL)
        return -1;

	if(codeve & FD_ACCEPT)
	{
		#ifdef LOGAR_SOCKET
			logar("FD_ACCEPT");
		#endif
        ns=pai->slots.aloca();
        if(ns<0)
        {
            con->recusar();
            logar("Conex�o recusada");
            return 0;
        }
        s=pai->slots[ns];
		s->_reset();	//TODO: _reset() naum eh publico
        s->c=con->aceitar();
        if(s->c!=NULL)
        {
            s->c->pai=con->pai;
            s->c->id=ns;
            s->c->registraCallback(tratar);
        }
	}
	if(codeve & FD_CLOSE)
		return 1;	//matar thread...
	return 0;
}