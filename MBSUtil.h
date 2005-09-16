#ifndef mbsutilH
#define mbsutilH

#include "Parser.h"
#include "Tempo.h"

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
