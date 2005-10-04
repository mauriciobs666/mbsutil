#ifndef ThreadH
#define ThreadH

#include "windows.h"

typedef void*(*Callback)(void*);	//ponteiro pra funcao com assinatura:
									//void* funcao(void*)
/*
	Return Values - WaitForSingleObject()

	If the function fails, the return value is WAIT_FAILED.
	
	If the function succeeds, the return value indicates the event that caused 
		the function to return:

	Value			Meaning
	WAIT_ABANDONED	The specified object is a mutex object that was not released 
					by the thread that owned the mutex object before the owning 
					thread terminated. Ownership of the mutex object is granted 
					to the calling thread, and the mutex is set to nonsignaled.
	WAIT_OBJECT_0	The state of the specified object is signaled.
	WAIT_TIMEOUT	The time-out interval elapsed, and the object's state is 
					nonsignaled.
*/

class Mutex
{
	HANDLE hnd;
public:
    Mutex() { hnd=CreateMutex(NULL,false,NULL); }
    ~Mutex() { CloseHandle(hnd); }
    unsigned long trava() { return WaitForSingleObject(hnd,INFINITE); }
    unsigned long destrava() { return ReleaseMutex(hnd); }
    unsigned long estado() { return WaitForSingleObject(hnd,0); }
};

class Thread
{
	HANDLE hnd;
	DWORD id;
	void* retorno;
public:
	Thread(Callback funcao, void *parametro, bool iniciar);
	Thread(DWORD WINAPI (*t)(LPVOID), LPVOID parametro, bool iniciar=true);
	~Thread();
	void* pegaRetorno();
	int resume();
	int pause();
	int stop();
	int kill();
};

#endif
 
