#ifndef mbsutilH
#define mbsutilH

#include <stdlib.h>

#define EVE_TEMPO_1S	0x0001
#define EVE_TEMPO_100MS	0x0002

class Tempo
{
	public:
     	Tempo();
     	Tempo(unsigned long quaisEventos);
    	void loop();
    	void reset();
    	unsigned long trataEvento();
    	unsigned long getFrames();
    	unsigned long getFPS();
    	unsigned long getFPSMedio();
    	unsigned long getTempoTotal();
    protected:
		unsigned long logarEventos;
		unsigned long ttotal;
	    unsigned long ultimo,agora;
     	unsigned long T1ds;
		unsigned long frames;
		unsigned long fpstemp,fps;
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

class Parser
{
	public:
	    char chars[256];//tabela 
        char linha[256];//scratch (gratis huahuahuahuahu)

        int tokenTipo;
        char token[256];
        double tokenNum;//

		Parser();
		~Parser();
        void setaLinha(char *ln);   //seta ponteiro pra linha a ser analizada
        void setaLinha();           //analisa linha[];
        int pegaToken();            //devolve tokenTipo
        int parseFloat(float *pnt, int qtd);	//le floats[qtd]
	private:
        char *l;        //ponteiro temp pra linha sendo analizada
        char *tok;      //ponteiro temp pro token sendo gerado
		bool isdelim(char c);
};

template<class C>class ListaL
{
public:
	ListaL();
	ListaL(const ListaL& l);
	int add(C *novo);
	int rm(C *item);
	int elementos();
protected:
	struct Noh
	{
		C *obj;
		Noh *prox;
		Noh(C *o) : obj(o) {prox=NULL;}
	};
	Noh *head;
	int nElementos;
};

template<class C> ListaL<C>::ListaL()
{
	head=NULL;
	nElementos=0;
}

template<class C> int ListaL<C>::add(C *novo)
{
	Noh *n=new Noh(novo);
	n->prox=head;
	head=n;
	nElementos++;
}

template<class C> int ListaL<C>::rm(C *item)
{
	Noh *tmp,*tmp2;
	if(head)
	{
		if(head->obj==item)
		{
			tmp=head;
			head=head->prox;
			delete tmp;
			nElementos--;
			
			return 0;
		}
		else
		{
			Noh *tmp2=head->prox;
			while(tmp2)
				if(tmp2->obj==item)
				{
					tmp=tmp2;
					tmp2=tmp2->prox;
					delete tmp;
					nElementos--;
					return 0;
				}
		}
	}
	return -1;
}

#endif
