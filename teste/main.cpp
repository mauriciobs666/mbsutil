#include <windows.h>
#include <cstdlib>
#include <iostream>
#include "../mbsutil.h"

using namespace std;

class abc
{
public:
	int dado;
	abc(int d): dado(d) {}
};

class thread
{
	private:
		bool sair;
		static DWORD WINAPI t(LPVOID tThis);
		int dado;
		HANDLE hnd;
	public:
		DWORD id;
		thread();
		~thread();
};

thread::thread()
{
	sair=false;
	dado=0;
	hnd=CreateThread(NULL,0,thread::t,(LPVOID)this,0,&id);
}

thread::~thread()
{

	sair=true;
	int retorno=WaitForSingleObject(hnd,1000);
	if(retorno==WAIT_OBJECT_0)
		printf("\nThread(%lu) sinalizada normalmente (WAIT_OBJECT_0)\n",id);
	else if(retorno==WAIT_TIMEOUT)
	{
		printf("\nThread(%lu) nao sinalizada (WAIT_TIMEOUT)\n",id);
	}
	CloseHandle(hnd);
}

DWORD WINAPI thread::t(LPVOID tThis)
{
	thread *este=(thread*)tThis;
	for(int x=0;x<25;x++)
	{
		este->dado++;
		printf("%d ", este->dado);
		if(este->sair)
			break;
		Sleep(200);
	}
	return (DWORD)0;
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
    return EXIT_SUCCESS;
}

int testeThread()
{
	thread *teste;
	teste=new thread();
//	printf("Objeto criado (ID=%d), pressione qualquer tecla pra finalizar...\n",teste->id);
	system("PAUSE");
	delete teste;
	printf("Objeto deletado\n");
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

int main(int argc, char *argv[])
{
	char op;
	cout << "Teste da MBSUtil" << endl;
	cout << "1 - testeThread()" << endl;
	cout << "2 - testeSoquete()" << endl;
	cout << "3 - testeSoqueteServer()" << endl;
	cout << "4 - testeExpressao()" << endl;
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
	}
    return EXIT_SUCCESS;
}
