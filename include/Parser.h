#ifndef MBSUTIL_PARSER_H
#define MBSUTIL_PARSER_H

#include <iostream>
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

class Variavel
{
public:
	virtual ~Variavel();
};

class VariavelDouble : public Variavel
{
};

class Expressao
{
public:
	Expressao() {}
	double eval(std::istream *ent);
	double eval(const std::string& s);
private:
	Lexer lexer;
	std::map<std::string,double> simbolos;
	double term(bool get) throw(string);
	double expr(bool get);
	double prim(bool get) throw(string);
};

std::string int2str(int i);
char asc2hex(char msb, char lsb);
#endif