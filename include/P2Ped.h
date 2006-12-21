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

#ifndef MBSUTIL_P2PED_H
#define MBSUTIL_P2PED_H

#include <iostream>
#include <map>
#include <list>
#include <queue>
#include <set>
#include <cstdio>
#include <string>

#include "Thread.h"
#include "Hash.h"
#include "P2Pdefs.h"

using std::istream;
using std::ostream;

//!Responsavel por armazenar informacoes sobre um endereco de rede TCP/IP.

class Noh
{
public:
	unsigned long ip;
	unsigned short porta;
	unsigned long id;

	Noh();
	bool idAlta() const
		{ return ((ip!=0)?(ip==id):false); }
	bool operator==(const Noh& n) const
		{ return ((ip==n.ip)&(porta==n.porta)&(id==n.id)); }
	bool operator<(const Noh& n) const
		{
			return (ip<n.ip) ? true : (ip>n.ip ? false :
				(porta<n.porta) ? true : (porta>n.porta ? false :
				(id<n.id) ? true : false) );
		}
	int read(unsigned char *&pkt);
	int write(unsigned char *&pkt) const;
    istream& read(istream& is);
    ostream& write(ostream& os) const;
};

class ListaNohs
{
public:
	void push(const Noh& no);
	Noh* pop(const Noh& no);
	int tamanho();
	void limpa();
    istream& read(istream& is);
    ostream& write(ostream& os);
private:
	Mutex m;
	std::list<Noh> lista;
};

//!Responsavel por descrever um cliente da rede P2P, protocolos aceitos etc.

class Cliente : public Noh
{
public:
	unsigned long versao;
	unsigned long opcoes;
	unsigned short mtu;
	unsigned short mru;
	unsigned long ping;

	Cliente();
	int read(unsigned char*& pkt);
	int write(unsigned char*& pkt) const;
	istream& read(istream& is);
    ostream& write(ostream& os);
};


//!Descreve um usuario da rede

class Usuario : public Hash128
{
public:
	char nick[Protocolo::TAMNICK];	//TODO: nick deveria ser string...
	unsigned long rx;				//volume de dados recebidos
	unsigned long tx;				//volume de dados enviados

	int classificacao() const
	{
		return rx-tx;
		//amigo=k
		//classificacao=saldo+amigo
	}

	unsigned long codMsg;			//id sequencial das mensagens (ACK)
	unsigned long codCli;			//codigo do noh (ou cliente) na lista de Nohs

	Usuario();
	Usuario(const Usuario& base);
	int mudaNick(std::string novo);

	int read(unsigned char*& pkt);
	int write(unsigned char*& pkt) const;
    istream& read(istream& is);
    ostream& write(ostream& os);
};

//!Fornece acesso facil e sincronizado para listas de usuarios
//IMPORTANTE: acessos diretos devem ser protegidos pelo mutex
class ListaUsuarios
{
public:
	Mutex m;
	std::map<Hash128,Usuario*> lista;

	ListaUsuarios() {}
	~ListaUsuarios() { limpa(); }

	bool insere(const Hash128& h, Usuario* u);
	void remove(const Hash128& h);
	Usuario* busca(const Hash128& h);
	int tamanho();
	void limpa();
    istream& read(istream& is);
    ostream& write(ostream& os);
};

//!Relaciona Usuarios com Nohs.
class ListaSessoes
{
public:
	ListaSessoes() {}
	~ListaSessoes() { limpa(); }

	bool insere(const Hash128& h, const Noh& n);
	void remove(const Hash128& h);
	Noh operator[](const Hash128& h);
	Hash128 operator[](const Noh& n);
	bool busca(const Hash128& h);
	bool busca(const Noh& n);
	int tamanho();
	void limpa();
private:
	Mutex m;
	std::map<Hash128,Noh> sessoes;
};

//!Fornece acesso facil e sincronizado para listas de hashs
//IMPORTANTE: acessos diretos devem ser protegidos pelo mutex
class ListaHash128
{
public:
	Mutex m;
	std::set<Hash128> lista;

	ListaHash128() {}
	~ListaHash128() { limpa(); }

	int insere(const Hash128& h);
	void remove(const Hash128& h);
	bool busca(const Hash128& h);
	int tamanho();
	void limpa();
    istream& read(istream& is);
    ostream& write(ostream& os);
};

#endif
