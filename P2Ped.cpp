#include "P2Ped.h"

using namespace std;
using namespace Protocolo;

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
