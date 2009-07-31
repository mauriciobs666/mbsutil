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

#include "Parser.h"
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <cmath>
#include <stdio.h>

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

Expressao::Expressao()
{
	funcoes.insert("acos");
	funcoes.insert("asin");
	funcoes.insert("atan");
	funcoes.insert("atan2");
	funcoes.insert("ceil");
	funcoes.insert("cos");
	funcoes.insert("cosh");
	funcoes.insert("exp");
	funcoes.insert("fabs");
	funcoes.insert("floor");
	funcoes.insert("fmod");
	funcoes.insert("ldexp");
	funcoes.insert("log");
	funcoes.insert("log10");
	funcoes.insert("pow");
	funcoes.insert("sin");
	funcoes.insert("sinh");
	funcoes.insert("sqrt");
	funcoes.insert("tan");
	funcoes.insert("tanh");
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

			if(funcoes.find(nome)!=funcoes.end())
				return func(nome, false);

        	if(simbolos.find(nome)==simbolos.end())		//se não declarado
        		if((DELIM!=lexer.atual.tipo)||("="!=lexer.atual.str))
					throw(string("Simbolo invalido"));

			double& v=simbolos[nome];
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
			throw(string("Token invalido: ")+lexer.atual.str);
		break;
	}
    throw(string("Primario esperado"));
}

double Expressao::func(string nome, bool get) throw(string)
{
	double args[32];
	int n=0;

	if(get)
		lexer.pegaToken();
	if((DELIM==lexer.atual.tipo)&&("("==lexer.atual.str))
	{
		do
		{
			args[n]=expr(true);
			n++;
		}
		while((lexer.atual.tipo==DELIM)&&(","==lexer.atual.str));

		if((lexer.atual.tipo!=DELIM)||(")"!=lexer.atual.str))
			throw(string(") esperado"));
		lexer.pegaToken();
		if(1==n)
		{
			if("acos"==nome)
				return acos(args[0]);
			else if("asin"==nome)
				return asin(args[0]);
			else if("atan"==nome)
				return atan(args[0]);
			else if("ceil"==nome)
				return ceil(args[0]);
			else if("cos"==nome)
				return cos(args[0]);
			else if("cosh"==nome)
				return cosh(args[0]);
			else if("exp"==nome)
				return exp(args[0]);
			else if("fabs"==nome)
				return fabs(args[0]);
			else if("floor"==nome)
				return floor(args[0]);
			else if("log"==nome)
				return log(args[0]);
			else if("log10"==nome)
				return log10(args[0]);
			else if("sin"==nome)
				return sin(args[0]);
			else if("sinh"==nome)
				return sinh(args[0]);
			else if("sqrt"==nome)
				return sqrt(args[0]);
			else if("tan"==nome)
				return tan(args[0]);
			else if("tanh"==nome)
				return tanh(args[0]);
		}
		else if(2==n)
		{
			if("atan2"==nome)
				return atan2(args[0], args[1]);
			else if("fmod"==nome)
				return fmod(args[0], args[1]);
			else if("ldexp"==nome)
				return ldexp(args[0], int(args[1]));
			else if("pow"==nome)
				return pow(args[0], args[1]);
		}
		throw(nome + string("(): Numero invalido de argumentos"));
	}
	else
		throw(string("( esperado"));
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
