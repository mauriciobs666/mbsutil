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
