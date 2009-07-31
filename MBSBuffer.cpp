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

#include "MBSBuffer.h"
#include <cstring>

using namespace std;

//------------------------------------------------------------------------------
//      MBSBuffer
//------------------------------------------------------------------------------

MBSBuffer::MBSBuffer(unsigned long tam)
{
	if(tam>0)
	{
		pntL=pntE=dados=new unsigned char[tam];
		if(dados!=NULL)
			tamanho=tam;
	}
	else
	{
		pntL=pntE=dados=NULL;
		tamanho=0;
	}
}

void MBSBuffer::reset()
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

int MBSBuffer::mudaTamanho(unsigned long tam)
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

unsigned long MBSBuffer::append(MBSBuffer& b, unsigned long qtd)
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

int MBSBuffer::readByte(unsigned char *uc)
{
	if(disponiveis()<1)
		return -1;
	*uc=*pntL++;
	return 0;
}

int MBSBuffer::writeByte(unsigned char uc)
{
	unsigned long novotam=ocupados()+1;
	if(novotam>tamanho)
		if(mudaTamanho(novotam))
			return -1;
	*pntE=uc;
	pntE++;
	return 0;
}

int MBSBuffer::readShort(unsigned short *us)
{
	if(disponiveis()<sizeof(unsigned short))
		return -1;
	*us=*((unsigned short*)pntL);
	pntL+=sizeof(unsigned short);
	return 0;
}

int MBSBuffer::writeShort(unsigned short us)
{
	unsigned long novotam=ocupados()+sizeof(unsigned short);
	if(novotam>tamanho)
		if(mudaTamanho(novotam))
			return -1;
	*((unsigned short*)pntE)=us;
	pntE+=sizeof(unsigned short);
	return 0;
}

int MBSBuffer::readLong(unsigned long *ul)
{
	if(disponiveis()<sizeof(unsigned long))
		return -1;
	*ul=*((unsigned long*)pntL);
	pntL+=sizeof(unsigned long);
	return 0;
}

int MBSBuffer::writeLong(unsigned long ul)
{
	unsigned long novotam=ocupados()+sizeof(unsigned long);
	if(novotam>tamanho)
		if(mudaTamanho(novotam))
			return -1;
	*((unsigned long*)pntE)=ul;
	pntE+=sizeof(unsigned long);
	return 0;
}

int MBSBuffer::readString(char *dest, unsigned long max)
{
	unsigned long tamstr=strlen((char*)pntL)+1;
	if(disponiveis()<tamstr)
		return -1;
	if(max<tamstr)
		return -2;
	memcpy(dest,pntL,tamstr);
	pntL+=tamstr;
	return 0;
}

int MBSBuffer::writeString(const char *orig, unsigned long max)
{
	//TODO: limit size
	int tamstr=strlen(orig)+1;
	unsigned long novotam=ocupados()+tamstr;
	if(novotam>tamanho)
		if(mudaTamanho(novotam))
			return -1;

	memcpy(pntE,orig,tamstr);
    pntE+=tamstr;

    return 0;
}
