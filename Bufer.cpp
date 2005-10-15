#include "Bufer.h"
#include <stdio.h>
#include <string.h>

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
