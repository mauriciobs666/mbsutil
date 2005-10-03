#ifndef MBSUTIL_PARSER_H
#define MBSUTIL_PARSER_H

#include <iostream>
#include <map>

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

class Parser
{
public:
	struct Token
	{
		Tipo tipo;
		double num;
		std::string str;
	} atual;
	Parser(std::istream *ent=NULL) : input(ent) {}
	~Parser() {}
	void setaEntrada(std::istream *ent) { input=ent; }
	Tipo pegaToken();
protected:
	std::istream *input;
};

class Expressao
{
public:
	Expressao() {}
	double eval(std::istream *ent);
	double eval(const std::string& s);
private:
	Parser parser;
	std::map<std::string,double> tabela;
	double term(bool get);
	double expr(bool get);
	double prim(bool get);
};

//public:
#define PRS_ERRO    -1  //erro interno do parser
#define PRS_NULL    0   //token(s) indisponivei(s) (fim da string)
#define PRS_NUM     1   //numero (float ['-']('0'-'9')*'.'['0'-'9']*)
#define PRS_STR     2   //string (char[]) 
#define PRS_DELIM   3   //delimitador, um char em token[0] -> (!isalpha()) && (!isdigit())

//private:
#define PRS_INICIO  11  //estado inicial
#define PRS_FIM     12  //aceitaçao 

class Parser_tosco
{
	public:
        char linha[256];//scratch (gratis huahuahuahuahu)

        int tokenTipo;
        char token[256];
        double tokenNum;//

		Parser_tosco();
		~Parser_tosco();
        void setaLinha(char *ln);
        void setaLinha();
        int pegaToken();            //devolve tokenTipo
	private:
        char *l;        //ponteiro temp pra linha sendo analizada
        char *tok;      //ponteiro temp pro token sendo gerado
		bool isdelim(char c);
};

#endif
