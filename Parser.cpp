#include "Parser.h"
#include <ctype.h>
#include <stdlib.h>

Parser::Parser()
{
	setaLinha();
}

Parser::~Parser()
{
}

void Parser::setaLinha(char *ln)
{
	l=ln;
}

void Parser::setaLinha()
{
	l=linha;
}

bool Parser::isdelim(char c)
{
    if(isalpha(c)|isdigit(c))
	    return false;
    return true;
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
