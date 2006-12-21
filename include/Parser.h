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

#ifndef MBSUTIL_PARSER_H
#define MBSUTIL_PARSER_H

#include <iostream>
#include <set>
#include <map>

using std::string;

enum Tipo
{
			//	Tipos de tokens:
			//	----------------
	ERRO=-1,//	Erro interno.
	NULO=0,	//	Fim da stream.
	NUM,	//	Numero no formato definido pelo padrao ISO C++.
	NOME,	//
	DELIM	//	Qualquer caracter que nao se enquadre nas categorias acima.
};

class Lexer
{
public:
	struct Token
	{
		Tipo tipo;
		double num;
		string str;
	} atual;
	Lexer(std::istream *ent=NULL) : input(ent) {}
	~Lexer() {}
	void setaEntrada(std::istream *ent) { input=ent; }
	Tipo pegaToken();
	Tipo pegaString();	//strings separadas por isspace()
protected:
	std::istream *input;
};

class Expressao
{
public:
	Expressao();
	double eval(std::istream *ent);
	double eval(const std::string& s);
private:
	Lexer lexer;
	std::map<std::string,double> simbolos;
	std::set<std::string> funcoes;
	double term(bool get) throw(string);
	double expr(bool get);
	double prim(bool get) throw(string);
	double func(std::string nome, bool get) throw(string);
};

std::string int2str(int i);
char asc2hex(char msb, char lsb);
#endif
