#ifndef MBSUTIL_FILA_H
#define MBSUTIL_FILA_H

template<class C>class Fila
{
public:
    Fila();
    ~Fila();
    int insere(C *dado);
    C* remove();
protected:
    struct Noh
    {
        Noh *prox;
        C *obj;
        Noh(C *o) : obj(o) { prox=NULL; }
    } *pri, *ult;
};

template<class C>Fila<C>::Fila()
{
    pri=ult=NULL;
}

template<class C>Fila<C>::~Fila()
{
	//TODO: Destrutor de Fila
}

template<class C>int Fila<C>::insere(C *dado)
{
	Noh *novo=new Noh(dado);
	if(pri==NULL)
		pri=ult=novo;
	else
	{
		ult->prox=novo;
		ult=novo;
	}
	return 0;
}

template<class C>C* Fila<C>::remove()
{
	C *dado;
	Noh *este;
	if(pri==NULL)
		return NULL;
	este=pri;
	dado=este->obj;
	pri=este->prox;
	if(pri==NULL)
		ult=NULL;
	delete este;
	return dado;
}

#endif
