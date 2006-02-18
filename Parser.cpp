#include "Parser.h"
#include <cstdlib>
#include <map>
#include <cctype>
#include <sstream>

using namespace std;

Tipo Lexer::pegaToken()
{
	char ch;
	do
	{
		if(!input->get(ch))
			return atual.tipo=NULO;
	} while(ch!='\n' && isspace(ch));

	switch(ch)
	{
		case 0:
			return atual.tipo=NULO;
		case '0':	case '1':	case '2':	case '3':	case '4':	case '5':
		case '6':	case '7':	case '8':	case '9':	case '.':
			input->putback(ch);
			*input >> atual.num;
			return atual.tipo=NUM;
		default:
			atual.str=ch;
			if(isalpha(ch))
			{
				while(input->get(ch)&&isalnum(ch))
					atual.str.push_back(ch);
				input->putback(ch);
				return atual.tipo=NOME;
			}
		return atual.tipo=DELIM;
	}
}

Tipo Lexer::pegaString()
{
	char ch;
	do
	{
		if(!input->get(ch))
			return atual.tipo=NULO;
	} while(ch!='\n' && isspace(ch));
	atual.str=ch;
	while(input->get(ch)&&!isspace(ch))
		atual.str.push_back(ch);
	input->putback(ch);
	return atual.tipo=NOME;
}

double Expressao::eval(const string& s)
{
	istringstream *iss=new istringstream(s);
	double ret=eval(iss);
	delete iss;
	return ret;
}

double Expressao::eval(istream *ent)
{
	lexer.setaEntrada(ent);
	double ret=expr(true);
	lexer.setaEntrada(NULL);
	return ret;
}

double Expressao::term(bool get) throw(string)
{
	double esq=prim(get);
	for(;;)
	{
		if(DELIM==lexer.atual.tipo)
		{
			if("*"==lexer.atual.str)
				esq*=prim(true);
			else if("/"==lexer.atual.str)
			{
				if(double d=prim(true))
					esq/=d;
				else
					throw(string("Divisao por zero"));
			}
			else
				return esq;
		}
		else
			return esq;
	}
}

double Expressao::expr(bool get)
{
	double esq=term(get);
	for(;;)
		if(DELIM==lexer.atual.tipo)
		{

			if("+"==lexer.atual.str)
				esq+=term(true);
			else if("-"==lexer.atual.str)
				esq-=term(true);
			else
				return esq;
		}
		else
			return esq;
}

double Expressao::prim(bool get) throw(string)
{
	double d;

	if(get)
		lexer.pegaToken();
	switch(lexer.atual.tipo)
	{
		case NUM:
			d=lexer.atual.num;
			lexer.pegaToken();
		return d;
		case NOME:
        {
        	string nome=lexer.atual.str;
        	lexer.pegaToken();

        	if(simbolos.find(nome)==simbolos.end())		//se não declarado
        		if((DELIM!=lexer.atual.tipo)||("="!=lexer.atual.str))
					throw(string("Simbolo invalido"));

			double& v=simbolos[lexer.atual.str];
			if((DELIM==lexer.atual.tipo)&&("="==lexer.atual.str))
				v=expr(true);
		    return v;
        }
        break;
		case DELIM:
			if("-"==lexer.atual.str)		//menos unario
				return -prim(true);
			else if("("==lexer.atual.str)
			{
				d=expr(true);
				if((lexer.atual.tipo!=DELIM)||(")"!=lexer.atual.str))
					throw(string(") esperado"));
				lexer.pegaToken();
				return d;
			}
			else
				throw(string("Operacao invalida"));
		break;
		default:
			throw(string("Token invalido"));
		break;
	}
    throw(string("Primario esperado"));
}

string int2str(int i)
{
	char temp[32];
	sprintf(temp,"%i",i);
	return string(temp);
}

char asc2hex(char msb, char lsb)
{
	msb=toupper(msb);
	lsb=toupper(lsb);
	char tmp=(msb<='9')?(msb-'0'):((msb-'A')+10);
	tmp<<=4;
	tmp+=(lsb<='9')?(lsb-'0'):((lsb-'A')+10);
	return tmp;
}
