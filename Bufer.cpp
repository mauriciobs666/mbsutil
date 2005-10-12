#include "Bufer.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
//      Buffer
//------------------------------------------------------------------------------

Buffer::Buffer(unsigned long tam)
{
	ocupados=tamanho=0;
	if(tam>0)
	{
		dados=new unsigned char[tam];
		if(dados!=NULL)
			tamanho=tam;
	}
	resetPnt();
}

void Buffer::resetPnt()
{
    if((tamanho==0)|(dados==NULL))
    {
        tamanho=ocupados=0;
        if(dados!=NULL)
			delete[] dados;
        pntE=pntL=dados=NULL;
        return;
    }
    if(ocupados>tamanho)
        ocupados=tamanho;
    pntL=dados;
    pntE=dados;
    pntE+=ocupados;
}

unsigned long Buffer::faltaTratar() const
{
    if(ocupados==0)
        return 0;
    if(pntL<pntE)
        return (pntE-pntL);
    return 0;
}

unsigned long Buffer::append(Buffer *b, unsigned long qtd)
{
    if((qtd==0)|(qtd>b->faltaTratar()))
        qtd=b->faltaTratar();
    unsigned long novotam=ocupados+qtd;
    if(novotam>tamanho)
        if(mudaTamanho(novotam))
			return 0;
    ocupados=novotam;
    memcpy(pntE,b->pntL,qtd);
    b->pntL+=qtd;
    pntE+=qtd;
    return qtd;
}

int Buffer::mudaTamanho(unsigned long tam)
{
    unsigned char *novo,*tmp;
    int leitura=pntL-dados;
	
    if((novo=new unsigned char[tam])==NULL)
        return -1;
    if(tamanho>0)
        memcpy(novo,dados,tamanho);
    tmp=dados;
    dados=novo;
    delete[] tmp;
    tamanho=tam;
    pntE=dados;
    pntE+=ocupados;
    pntL=dados;
    pntL+=leitura;
    return 0;
}
