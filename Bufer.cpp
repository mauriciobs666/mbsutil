#include "Bufer.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
//      Buffer
//------------------------------------------------------------------------------

Buffer::Buffer()
{
    tamanho=ocupados=0;
    pntE=pntL=dados=NULL;
}

Buffer::Buffer(int tam)
{
    ocupados=0;
    tamanho=aloca(dados,tam);
    pntL=pntE=dados;
}

Buffer::~Buffer()
{
    mata(dados);
}

void Buffer::resetPnt()
{
    if((tamanho==0)|(dados==NULL))
    {
        tamanho=ocupados=0;
        pntE=pntL=dados=NULL;
        return;
    }
    if(ocupados<0)
        ocupados=0;
    if(ocupados>tamanho)
        ocupados=tamanho;
    pntL=dados;
    pntE=dados;
    pntE+=ocupados;
}

int Buffer::faltaTratar()
{
    if(ocupados<=0)
        return 0;
    if(pntL<pntE)
        return (pntE-pntL);
    return 0;
}

int Buffer::livres()
{
    return tamanho-ocupados;
}

int Buffer::append(Buffer *b)
{
    return append(b,b->faltaTratar());
}

int Buffer::append(Buffer *b, int qtd)
{
    if(qtd>b->faltaTratar())
        qtd=b->faltaTratar();
    if(qtd<=0)
        return 0;
    int novotam=ocupados+qtd;
    if(novotam>tamanho)
        mudaTamanho(novotam);
    ocupados=novotam;
    memcpy(pntE,b->pntL,qtd);
    b->pntL+=qtd;
    pntE+=qtd;
    return qtd;
}

unsigned long Buffer::pegaTamanho()
{
    return tamanho;
}

int Buffer::aloca(unsigned char *&end, int qtd)
{
    end=new unsigned char[qtd];
    if(end!=NULL)
        return qtd;
    return 0;
}

void Buffer::mata(unsigned char *&end)
{
    if(end)
    {
        delete[] end;
        end=NULL;
    }
}

int Buffer::mudaTamanho(int tam)
{
    unsigned char *novo,*tmp;
    int leitura=pntL-dados;

    if(aloca(novo,tam)!=tam)
    {
        mata(novo);
        return -1;
    }
    if(tamanho>0)
        memcpy(novo,dados,tamanho);
    tmp=dados;
    dados=novo;
    mata(tmp);
    tamanho=tam;
    pntE=dados;
    pntE+=ocupados;
    pntL=dados;
    pntL+=leitura;
    return 0;
}
