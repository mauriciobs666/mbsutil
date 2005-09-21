#include <cstdlib>
#include <iostream>
#include "../mbsutil/mbsutil.h"

using namespace std;

class abc
{
	public:
		int dado;
		abc(int d): dado(d) {}
};

int testeFila()
{
	Fila<abc> f;
	abc *m;
	printf("jogando na fila de 0 a 9...\n");
	for(int x=10;x>0;x--)
	{
		printf("%d ",x);
		m=new abc(x);
		f.insere(m);
	}
	printf("\ntirando da fila...\n");
	while((m=f.remove())!=NULL)
	{
		printf("%d ",m->dado);
		delete m;
	}
	return 0;
}

int testeLista()
{
	ListaL<int> lst;
	int *novo=new int[10];

	for(int x=0;x<10;x++)
	{
		novo[x]=x;
		cout << novo[x] << endl;
		lst.add(&novo[x]);
	}

	for(int x=0;x<10;x++)
	{
		lst.rm(&novo[9-x]);
		cout << "Elementos=" << lst.elementos() << endl;
	}
	delete[] novo;
	return 0;
}

int main(int argc, char *argv[])
{
	testeFila();
	testeLista();
	printf("\n");	
    system("PAUSE");
    return EXIT_SUCCESS;
}
