#include <windows.h>
#include <conio.h>
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
	char dados[50];
	MBSSocket cli;
	int retorno=cli.conectar("localhost",6661);
	if(retorno==0)
	{
		cout << "Conectado" << endl;
		cli.enviar("oi mundo",strlen("oi mundo")+1);
		if(cli.receive(dados,20)==0)
			cout << "Desconectado normalmente";
		else
			cout << "Recebido: " << dados << endl;
	}
	else
		cout << "Nao conectou" << endl;
}

int testeSoqueteServer()
{
	MBSSocketServer ss;
	char temp[50];
	cout << "Soquete::ouvir(6661)=" << ss.ouvir(6661) << endl;
	MBSSocket *s=ss.aceitar();
	if(s==NULL)
		cout << "Erro" << endl;
	else
	{
		int rec;
		cout << "Conectado" << endl;
		while(s->valid())
		{
			rec=s->receive(temp,50);
			if (rec>0)
			{
				cout << "Recebido: " << temp << endl;
				s->enviar(temp,rec);
			}
			else if(rec==0)
			{
				cout << "desconectando" << endl;
				s->disconnect();
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

int testePath()
{
	Path tmp;
	list<string> lst=tmp.find("../*");
	for(list<string>::iterator i=lst.begin();i!=lst.end();i++)
		cout << *i << endl;
}

int testeSocket()
{
	char dados[20];
	MBSSocket cli;
	MBSSocketSelector sel;
	int retorno=cli.conectar("localhost",6661);
	if(retorno==0)
	{
		sel.add(cli.fd);

		cout << "Conectado" << endl;
		cout << "cli.enviar=" << cli.enviar("oi mundo",strlen("oi mundo")+1) << endl;
		while(true)
		{
			int rc=sel.Select();
			if(rc<0)
				cout << "sel error=" << rc << endl;
			if(sel.isRead(cli.fd))
			{
				rc=cli.receive(dados,50);
				if(rc>0)
					cout << "Recebido: " << dados << endl;
				else if(rc==0)
				{
					cout << "desconectando normalmente" << endl;
					cli.disconnect();
					break;
				}
				else
				{
					cout << "cli.receive=" << rc << endl;
					cli.disconnect();
					break;
				}
			}
			if(sel.isException(cli.fd))
			{
				cout << "Exception cli" << endl;
				cli.disconnect();
				break;
			}
		}
	}
	else
		cout << "Nao conectou" << endl;
}

int testeSocketServer()
{
	char temp[50];
	MBSSocketServer ss;
	MBSSocketSelector sel;
	MBSSocket *cli=NULL;

	cout << "Soquete::ouvir(6661)=" << ss.ouvir(6661) << endl;
	if(!ss.valid())
	{
		cout << "Error listening" << endl;
		return -1;
	}
	sel.add(ss.fd);

	int rc;
	for(;;)
	{
		rc=sel.Select();
		if(rc<0)
			cout << "sel error=" << rc << " wsa=" << WSAGetLastError() << endl;
		if(sel.isRead(ss.fd))
		{
			if(cli==NULL)
			{
				cli=ss.aceitar();
				if(cli==NULL)
					cout << "Erro" << endl;
				else
				{
					cout << "Conectado" << endl;
					sel.add(cli->fd);
				}
			}
			else
			{
				cout << "Recusado" << endl;
				ss.refuse();
			}
		}
		if(sel.isException(ss.fd))
			cout << "Exception ss" << endl;

		if((cli!=NULL) && sel.isRead(cli->fd))
		{
			int rec=cli->receive(temp,50);
			if(rec>0)
			{
				cout << "Recebido: " << temp << endl;
				cli->enviar(temp,rec);
			}
			else if(rec==0)
			{
				cout << "desconectando" << endl;
				sel.remove(cli->fd);
				cli->disconnect();
				cli=NULL;
			}
			else
			{
				cout << "rec=" << rec << endl;
				sel.remove(cli->fd);
				cli->disconnect();
				cli=NULL;
			}
		}
		if((cli!=NULL) && sel.isException(cli->fd))
		{
			cout << "Exception cli" << endl;
			sel.remove(cli->fd);
			cli->disconnect();
			cli=NULL;
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
	cout << "5 - testeHash()" << endl;
	cout << "6 - testeConversao()" << endl;
	cout << "7 - testePath()" << endl;
	cout << "8 - testeMBSSocket()" << endl;
	cout << "9 - testeMBSSocketServer()" << endl;
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
		case '7':
			testePath();
		break;
		case '8':
			testeSocket();
		break;
		case '9':
			testeSocketServer();
		break;
	}
	getch();
    return EXIT_SUCCESS;
}
