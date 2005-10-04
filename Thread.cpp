#include "Thread.h"


Thread::Thread(Callback funcao, void *parametro, bool iniciar)
{
//	int flags=0;
//	if(!iniciar)
//		flags=CREATE_SUSPENDED;
//	if(funcao==NULL)
//		return;
//	hnd=CreateThread(NULL,0,funcao,parametro,flags,&id);
}


Thread::Thread(DWORD WINAPI (*t)(LPVOID), LPVOID parametro, bool iniciar)
{
	int flags=0;
	if(!iniciar)
		flags=CREATE_SUSPENDED;
	if(t==NULL)
		return;
	hnd=CreateThread(NULL,0,t,parametro,flags,&id);
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
