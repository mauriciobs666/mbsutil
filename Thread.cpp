#include "Thread.h"
/*
#include <stdio.h>
#ifdef _WIN32
#include "socket_include.h"
#else
#include <unistd.h>
#endif
*/

Thread::Thread(bool iniciar)
{
	#ifdef _WIN32
		if(!iniciar)
			hnd=CreateThread(NULL,0,t,this,CREATE_SUSPENDED,&id);
		else
			hnd=CreateThread(NULL,0,t,this,0,&id);
	#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		if (pthread_create(&m_thread,&attr,StartThread,this) == -1)
		{
			perror("Thread: create failed");
			SetRunning(false);
		}
	#endif
}

Thread::~Thread()
{
/*
	if (m_running)
	{
		SetRunning(false);
		SetRelease(true);

#ifdef _WIN32
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		select(0,NULL,NULL,NULL,&tv);
		::CloseHandle(m_thread);
#else
		sleep(1);
#endif
	}
*/
	if(hnd!=NULL)
	{
		CloseHandle(hnd);
	}
}

/*
threadfunc_t STDPREFIX Thread::StartThread(threadparam_t zz)
{
	Thread *pclThread = (Thread *)zz;

	while (pclThread -> m_running && !pclThread -> m_release)
	{
#ifdef _WIN32
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		select(0,NULL,NULL,NULL,&tv);
#else
		sleep(1);
#endif
	}
	if (pclThread -> m_running)
	{
		pclThread -> Run();
	}
	pclThread -> SetRunning(false); // if return
	return (threadfunc_t)NULL;
}
*/

static THREAD_RET THREAD_PRE t(THREAD_PARM p)
{
	Thread *esta=(Thread*)p;
	esta->executando(true);
	esta->run();
	esta->executando(false);
	return (THREAD_RET)NULL;
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
