#ifndef mbsutilH
#define mbsutilH

#include "Parser.h"
#include "Tempo.h"

template<class C>class ListaL
{
public:
	ListaL();
	ListaL(const ListaL& l);
	~ListaL();
	int add(C *novo);
	int rm(C *item);
	int elementos() { return nElementos; }
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

template<class C> ListaL<C>::~ListaL()
{
	Noh *tmp;
	while(head)
	{
		tmp=head->prox;
		delete head;
		head=tmp;
	}
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
	Noh *tmp=head;
	if(tmp)		//lista vazia?
	{
		Noh *tmp2=tmp->prox;
		if(tmp->obj==item)
		{
			head=tmp2;
			delete tmp;
			nElementos--;
			return 0;
		}
		else
		{
			while(tmp2)
			{
				if(tmp2->obj==item)
				{
					tmp->prox=tmp2->prox;
					delete tmp2;
					nElementos--;
					return 0;
				}
				tmp=tmp2;
				tmp2=tmp2->prox;
			}
		}
	}
	return -1;
}
#endif
