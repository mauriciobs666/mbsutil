#include <ctype.h>
#include <math.h>
#include <windows.h>
#include "mbsutil.h"

Tempo::Tempo()
{
	reset();
	logarEventos=EVE_TEMPO_100MS | EVE_TEMPO_1S;
}

Tempo::Tempo(unsigned long quaisEventos)
{
	reset();
	logarEventos=quaisEventos;
}

void Tempo::reset()
{
	ultimo=agora=GetTickCount();
	ttotal=T1ds=0;				//segundo atual e decimos do segundo atual
	frames=fps=fpstemp=0;
}

void Tempo::loop()
{
	frames++;
	fpstemp++;
	agora=GetTickCount();
}

unsigned long Tempo::getFrames()
{
	return frames;
}

unsigned long Tempo::getFPS()
{
	return fps;
}

unsigned long Tempo::getFPSMedio()
{
	if(ttotal>0)
		return (frames/ttotal);
	return 0;
}
    	
unsigned long Tempo::trataEvento()
{
	if(T1ds>=10)
  	{	//uma a cada 1 seg
  		T1ds-=10;
  		ttotal++;
		fps=fpstemp;
		fpstemp=0;
		if(logarEventos&EVE_TEMPO_1S)
			return EVE_TEMPO_1S;
  	}
	if((agora-ultimo)>100)
	{	//uma a cada 100ms
		ultimo+=100;
  		T1ds++;
  		if(logarEventos&EVE_TEMPO_100MS)
	  		return EVE_TEMPO_100MS;
	}
	return 0;
}

unsigned long Tempo::getTempoTotal()
{
	return ttotal;
}

Parser::Parser()
{
    setaLinha();
}

Parser::~Parser()
{
}

bool Parser::isdelim(char c)
{
    if(isalpha(c)|isdigit(c))
	    return false;
    return true;
}

void Parser::setaLinha(char *ln)
{
    l=ln;
}

void Parser::setaLinha()
{
    l=linha;
}

int Parser::pegaToken()
{
    tokenTipo=PRS_NULL;
    tok=token;
    char c;

    while(isspace(*l))              //remove espacos,tabs,CR,LF
        l++;

    int estado=PRS_INICIO;

    do
    {
        c=*l++;

        if(c==0)                        //fim da string
        {
            l--;
            estado=PRS_FIM;
        }

        if(estado==PRS_INICIO)          //estado inicial
        {
            if(isdigit(c))          //numero
                tokenTipo=estado=PRS_NUM;
            else if(isalpha(c))     //strings comecam com [A-Z] | [a-z]
                tokenTipo=estado=PRS_STR;
            else
            {
                tokenTipo=PRS_DELIM;
                estado=PRS_FIM;
            }
            *tok++=c;
        }
        else if(estado==PRS_NUM)
        {
            if(((!isspace(c))&(!isdelim(c)))|(c=='.'))
                *tok++=c;
            else
            {
                l--;
                estado=PRS_FIM;
            }
        }
        else if(estado==PRS_STR)
        {
            if((!isspace(c))&(!isdelim(c)))
                *tok++=c;
            else
            {
                l--;
                estado=PRS_FIM;
            }
        }
    } while(estado!=PRS_FIM);       //fim do token

    *tok=0;

    if(tokenTipo==PRS_NUM)
        tokenNum=(float)strtod(token,NULL);

    return tokenTipo;
}

int Parser::parseFloat(float *pnt, int qtd)
{
    float sinal;
	for(int x=0;x<qtd;x++)
	{
	    sinal=1.0f;
	    pegaToken();
	    if(tokenTipo==PRS_DELIM)
	    {
	        if(token[0]=='-')
	        	sinal=-1.0f;	        
	        pegaToken();
	    }
		if(tokenTipo!=PRS_NUM)
			return PRS_NULL;
		*pnt++=tokenNum*sinal;
	}    
	return PRS_NUM;
}
