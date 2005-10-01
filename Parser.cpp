#include "Parser.h"
#include <cstdlib>
#include <map>
#include <cctype>
#include <sstream>

using namespace std;

Tipo Parser::pegaToken()
{
	char ch;
	do
	{
		if(!input->get(ch))
			return atual.tipo=FIM;
	} while(ch!='\n' && isspace(ch));

	switch(ch)
	{
		case 0:
			return atual.tipo=FIM;
		case ';':	case '\n':
			return atual.tipo=IMPR;
		case '*':	case '/':	case '+':	case '-':	case '(':	case ')':
		case '=':
			return atual.tipo=Tipo(ch);
		case '0':	case '1':	case '2':	case '3':	case '4':	case '5':
		case '6':	case '7':	case '8':	case '9':	case '.':
			input->putback(ch);
			*input >> atual.num;
			return atual.tipo=NUMERO;
		default:
			if(isalpha(ch))
			{
				atual.str=ch;
				while(input->get(ch)&&isalnum(ch))
					atual.str.push_back(ch);
				input->putback(ch);
				return atual.tipo=NOME;
			}
//			erro("bad token");
			return atual.tipo=IMPR;
	}
}

double Expressao::eval(const string& s)
{
	return eval(new istringstream(s));
}

double Expressao::eval(istream *ent)
{
	parser.setaEntrada(ent);
	return expr(true);
}

double Expressao::term(bool get)
{
	double esq=prim(get);
	for(;;)
		switch(parser.atual.tipo)
		{
			case MUL:
				esq*=prim(true);
			break;
			case DIV:
				if(double d=prim(true))
				{
					esq/=d;
					break;
				}
				return 1;//erro("divisao por zero");
			default:
				return esq;
		}
}

double Expressao::expr(bool get)
{
	double esq=term(get);
	for(;;)
		switch(parser.atual.tipo)
		{
			case MAIS:
				esq+=term(true);
			break;
			case MENOS:
				esq-=term(true);
			break;
			default:
				return esq;
		}
}

double Expressao::prim(bool get)
{
	double n;
	double e;
	
	if(get)
		parser.pegaToken();
	switch(parser.atual.tipo)
	{
		case NUMERO:
			n=parser.atual.num;
			parser.pegaToken();
			return n;
		break;
		case NOME:
		{	
			double& v=tabela[parser.atual.str];
			if(parser.pegaToken()==ATRIB)
				v=expr(true);
			return v;
		}
		break;
		case MENOS:
			return -prim(true);
		case PE:
			e=expr(true);
			if(parser.atual.tipo!=PD)
				return 1;//erro(") esperado");
			parser.pegaToken();
			return e;
		default:
			return 1;//erro("primario esperado");
	}
}

Parser_tosco::Parser_tosco()
{
	setaLinha();
}

Parser_tosco::~Parser_tosco()
{
}

void Parser_tosco::setaLinha(char *ln)
{
	l=ln;
}

void Parser_tosco::setaLinha()
{
	l=linha;
}

bool Parser_tosco::isdelim(char c)
{
    if(isalpha(c)|isdigit(c))
	    return false;
    return true;
}

int Parser_tosco::pegaToken()
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
        tokenNum=strtod(token,NULL);

    return tokenTipo;
}
