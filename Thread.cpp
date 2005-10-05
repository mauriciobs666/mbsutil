#include "Thread.h"

DWORD WINAPI Thread::t(LPVOID p)
{
	Thread *esta=(Thread*)p;
	esta->ativa=true;	
	esta->retorno=esta->f(esta->fparm);
	esta->ativa=false;
	return 0;
}

Thread::Thread(Callback funcao, void *parametro, bool iniciar)
{
	int flags=0;
	if(!iniciar)
		flags=CREATE_SUSPENDED;
	if(funcao==NULL)
		return;
	f=funcao;
	fparm=parametro;
	hnd=CreateThread(NULL,0,t,this,flags,&id);
}

Thread::~Thread()
{
	if(hnd!=NULL)
	{
//		ExitThread();
		CloseHandle(hnd);
	}
}

int Thread::resume()
{
	if(hnd==NULL)
		return -1;
	return ResumeThread(hnd);
}

int Thread::pause()
{
	if(hnd==NULL)
		return -1;
	return SuspendThread(hnd);
}

/*
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
*/
