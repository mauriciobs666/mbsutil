#include <windows.h>
#include <cstdlib>
#include <iostream>
#include "mbsutil.h"

using namespace std;

void* thread(void*)
{
	for(int x=0;x<20;x++)
	{
		Sleep(500);
		cout << ".";
	}
	return NULL;
}

int testeConversao()
{
	cout << hex << (int)asc2hex('2','A');
}

int testeThread()
{
//	Thread teste(thread,NULL);
//	cout << "Esperando finalizacao da thread";
//	while(teste.executando());
//	cout << "ok." << endl;
	cout << "todo";
	return 0;
}

int testeSoquete()
{
	char dados[20];
	Soquete cli;
	int retorno=cli.conectar("127.0.0.1",80);
	if(retorno==0)
	{
		cout << "Conectado" << endl;
		cli.enviar("oi mundo",strlen("oi mundo")+1);
		cli.receber(dados,20);
		cout << "Recebido: " << dados << endl;
	}
	else
		cout << "Nao conectou" << endl;
}

int testeSoqueteServer()
{
	SoqueteServer ss;
	char temp[50];
	cout << "Soquete::ouvir(80)=" << ss.ouvir(80) << endl;
	Soquete *s=ss.aceitar();
	if(s==NULL)
		cout << "Erro" << endl;
	else
	{
		int rec;
		cout << "Conectado" << endl;
		while(s->conectado())
		{
			rec=s->receber(temp,50);
			if (rec>0)
			{
				cout << "Recebido: " << temp << endl;
				s->enviar(temp,rec);
			}
		}
	}
}

int testeExpressao()
{
	Expressao exp;
	string s="s=2*2+2";
	cout << "eval(" << s << ")" << endl;
	cout <<"s="<< exp.eval(s) << endl;
	cout << "CTRL-Z p/ finalizar" << endl;
	while(cin)
	{
		cout << "Expressao: ";
		cout << ": " << exp.eval(&cin) << endl;
	}
    return 0;
}

int testeHash()
{
	Hash128 a,b,c;
	cout << "a: " << a.toString() << endl;
	b=a;
	a.random();
	cout << "a: " << a.toString() << endl;
	cout << "b: " << b.toString() << endl;
	cout << "a==b " << (a==b) << endl;
	cout << "a<b " << (a<b) << endl;
	return 0;
}

int main(int argc, char *argv[])
{
	char op;
	cout << "Teste da MBSUtil" << endl;
	cout << "1 - testeThread()" << endl;
	cout << "2 - testeSoquete()" << endl;
	cout << "3 - testeSoqueteServer()" << endl;
	cout << "4 - testeExpressao()" << endl;
	cout << "5 - testeHash()" << endl;
	cout << "6 - testeConversao()" << endl;
	cout << endl << "Escolha uma opcao: ";
	cin >> op;
	switch(op)
	{
		case '1':
			testeThread();
		break;
		case '2':
			testeSoquete();
		break;
		case '3':
			testeSoqueteServer();
		break;
		case '4':
			testeExpressao();
		break;
		case '5':
			testeHash();
		break;
		case '6':
			testeConversao();
		break;
	}
    return EXIT_SUCCESS;
}
