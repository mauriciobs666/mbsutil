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

#include "Bufer.h"
#include <string>

using namespace std;

//------------------------------------------------------------------------------
//      Buffer
//------------------------------------------------------------------------------

Buffer::Buffer(unsigned long tam)
{
	if(tam>0)
	{
		dados=new unsigned char[tam];
		if(dados!=NULL)
			tamanho=tam;
	}
	else
	{
		dados=NULL;
		tamanho=0;
	}
	reset();
}

void Buffer::reset()
{
    if((tamanho==0)|(dados==NULL))
    {
        tamanho=0;
        if(dados!=NULL)
        {
			delete[] dados;
			dados=NULL;
        }
    }
    pntL=pntE=dados;
}

int Buffer::mudaTamanho(unsigned long tam)
{
    unsigned char *novo,*tmp;
    unsigned long leitura=pntL-dados;
    unsigned long escrita=pntE-dados;

    if((novo=new unsigned char[tam])==NULL)
        return -1;

    int copiar=(tamanho>tam)?(tam):(tamanho);

    if(copiar>0)
        memcpy(novo,dados,copiar);

    tmp=dados;
    dados=novo;
    delete[] tmp;

    tamanho=tam;
    if(escrita>tamanho) escrita=tamanho;
    if(leitura>tamanho) leitura=tamanho;
    pntE=dados+escrita;
    pntL=dados+leitura;
    return 0;
}

unsigned long Buffer::append(Buffer& b, unsigned long qtd)
{
    if((qtd==0)|(qtd>b.disponiveis()))
        qtd=b.disponiveis();
    unsigned long novotam=ocupados()+qtd;
    if(novotam>tamanho)
        if(mudaTamanho(novotam))
			return 0;
    memcpy(pntE,b.pntL,qtd);
    b.pntL+=qtd;
    pntE+=qtd;
    return qtd;
}

unsigned char Buffer::readByte()
{
	if(disponiveis()<1)
		return 0;
	return *pntL++;
}

unsigned long Buffer::writeByte(unsigned char uc)
{
	unsigned long novotam=ocupados()+1;
	if(novotam>tamanho)
		if(mudaTamanho(novotam))
			return 0;
	*pntE=uc;
	pntE++;
	return 1;
}

unsigned short Buffer::readShort()
{
	unsigned short us;
	if(disponiveis()<sizeof(us))
		return 0;
	us=*((unsigned short*)pntL);
	pntL+=sizeof(us);
	return us;
}

unsigned long Buffer::writeShort(unsigned short us)
{
	unsigned long novotam=ocupados()+sizeof(us);
	if(novotam>tamanho)
		if(mudaTamanho(novotam))
			return 0;
	*((unsigned short*)pntE)=us;
	pntE+=sizeof(us);
	return sizeof(us);
}

unsigned long Buffer::readLong()
{
	unsigned long ul;
	if(disponiveis()<sizeof(ul))
		return 0;
	ul=*((unsigned long*)pntL);
	pntL+=sizeof(ul);
	return ul;
}

unsigned long Buffer::writeLong(unsigned long ul)
{
	unsigned long novotam=ocupados()+sizeof(ul);
	if(novotam>tamanho)
		if(mudaTamanho(novotam))
			return 0;
	*((unsigned long*)pntE)=ul;
	pntE+=sizeof(ul);
	return sizeof(ul);
}
