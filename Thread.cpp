/*
	MBS-Util - General purpose C++ library
	Copyright (C) 2007 - Mauricio Bieze Stefani

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Thread.h"

/*
#include <stdio.h>
#ifdef _WIN32
#include "socket_include.h"
#else
#include <unistd.h>
#endif
*/

THREAD_RET THREAD_PRE t(THREAD_PARM p)
{
	Thread *esta=(Thread*)p;
	esta->executando(true);
	esta->run();
	esta->executando(false);
	return (THREAD_RET)NULL;
}

Thread::Thread(bool iniciar)
{
	finalizar=false;
	#ifdef _WIN32
		if(!iniciar)
			hnd=CreateThread(NULL,0,t,this,CREATE_SUSPENDED,&id);
		else
			hnd=CreateThread(NULL,0,t,this,0,&id);
	#else
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
		if (pthread_create(&hnd,&attr,t,this) == -1)
		{
			//perror("Thread: create failed");
			//SetRunning(false);
		}
	#endif
}

Thread::~Thread()
{
	stop();
}

#ifdef _WIN32
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

int Thread::stop()
{
	if(ativa)
	{
		finalizar=true;
//		int retorno=
		WaitForSingleObject(hnd,1000);
/*
		if(retorno==WAIT_OBJECT_0)
			printf("\nThread(%lu) sinalizada normalmente (WAIT_OBJECT_0)\n",id);
		else if(retorno==WAIT_TIMEOUT)
			printf("\nThread(%lu) nao sinalizada (WAIT_TIMEOUT)\n",id);
*/
		::CloseHandle(hnd);
		ativa=false;
		finalizar=false;
	}
	return 0;
}
#endif
