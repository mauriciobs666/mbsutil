#ifndef MBSUTIL_PARSER_H
#define MBSUTIL_PARSER_H

#include <iostream>
#include <map>

enum Tipo
{
	NOME, NUMERO, FIM, MAIS='+', MENOS='-',
	MUL='*', DIV='/', IMPR=';', ATRIB='=',
	PE='(', PD=')'
};

class Parser
{
	std::istream *input;
public:
	struct Token
	{
		double num;
		std::string str;
		Tipo tipo;
	} atual;
	
	Parser() {}
	~Parser() {}
	void setaEntrada(std::istream *ent) { input=ent; }
	Tipo pegaToken();
};

class Expressao
{
public:
	Parser parser;
	Expressao() {}
	double eval(std::istream *ent);
	double eval(const std::string& s);
private:
	std::map<std::string,double> tabela;
	double term(bool get);
	double expr(bool get);
	double prim(bool get);
};

//public:
#define PRS_ERRO    -1  //erro interno do parser
#define PRS_NULL    0   //token(s) indisponivei(s) (fim da string)
#define PRS_NUM     1   //numero (float ['-']('0'-'9')*'.'['0'-'9']*)
#define PRS_STR     2   //string (char[] 
#define PRS_DELIM   3   //delimitador, um char em token[0] -> (!isalpha()) && (!isdigit())

//private:
#define PRS_INICIO  11  //estado inicial
#define PRS_FIM     12  //aceitaçao 

class Parser_tosco
{
	public:
		enum Tipo { ERRO=-1, NULO=0, NUMERO, STRING, DELIM };
	    char chars[256];//tabela 
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
