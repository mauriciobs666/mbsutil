#ifndef MBSUTIL_THREAD_H
#define MBSUTIL_THREAD_H

#ifdef _WIN32
	#include <windows.h>
	typedef DWORD  THREAD_RET;
	typedef LPVOID THREAD_PARM;
	#define THREAD_PRE WINAPI
#else
	#include <pthread.h>
	typedef void * THREAD_RET;
	typedef void * THREAD_PARM;
	#define THREAD_PRE
#endif

class Mutex
{
public:
    Mutex()
    {
    	#ifdef _WIN32
			hnd=CreateMutex(NULL,false,NULL);
		#else
			pthread_mutex_init(&hnd,NULL);
		#endif
	}
    ~Mutex()
    {
    	#ifdef _WIN32
			::CloseHandle(hnd);
		#else
			pthread_mutex_destroy(&hnd);
		#endif
	}
    unsigned long trava()
    {
		#ifdef _WIN32
			return WaitForSingleObject(hnd,INFINITE);
		#else
			pthread_mutex_lock(&hnd);
			return 0;
		#endif
	}
    unsigned long destrava()
    {
		#ifdef _WIN32
			return ::ReleaseMutex(hnd);
		#else
			pthread_mutex_unlock(&hnd);
			return 0;
		#endif
	}
    unsigned long estado()
    {
		#ifdef _WIN32
			return WaitForSingleObject(hnd,0);	//TODO: equivalente posix
		#else
			return 0;
		#endif
	}
private:
	#ifdef _WIN32
		HANDLE hnd;
	#else
		pthread_mutex_t hnd;
	#endif
};

class Thread
{
public:
	Thread(bool iniciar=false);
	virtual ~Thread();

	virtual void run() = 0;
	int resume();
	int pause();
	int stop();
	int kill();
	bool executando()
		{ return ativa; }
	void executando(bool b)
		{ ativa=b; }
protected:
	bool ativa;
	bool finalizar;
private:
	#ifdef _WIN32
		HANDLE hnd;
		DWORD id;
	#else
		pthread_t hnd;
	#endif
};

#endif

/*	Introducao - sincronismo

Someone recently asked me what I recommend for synchronizing worker threads and
I suggested setting an event. This person's response was that you could not do
that since worker threads do not support a message pump (UI threads are required
to support messages). The confusion here is that events and messages are
different animals under windows.

Note there is considerable support for threads in MFC that is not covered here.
API's like _beginthread (a C runtime library call) would likely be replaced by
MFC API's like AfxBeginThread in an MFC application.
*/
/*	No Synchronization

This first example illustrates two unsynchronized threads. The main loop, which
is the primary thread of a process, prints the contents of a global array of
integers. The thread called "Thread" continuously populates the global array of
integers.

#include <process.h>
#include <stdio.h>

int a[ 5 ];

void Thread( void* pParams )
{
	int i;
	for(int num=0;;num++)
		for ( i = 0; i < 5; i++ )
			a[ i ] = num;
}

int main( void )
{
	_beginthread( Thread, 0, NULL );
	while( 1 )
		printf("%d %d %d %d %d\n", a[ 0 ], a[ 1 ], a[ 2 ], a[ 3 ], a[ 4 ] );
	return 0;
}

Note in this sample output, the numbers in red illustrate a state where the
primary thread preempted the secondary thread in the middle of populating the
values of the array:

81751652 81751652 81751651 81751651 81751651
81751652 81751652 81751651 81751651 81751651
83348630 83348630 83348630 83348629 83348629
83348630 83348630 83348630 83348629 83348629
83348630 83348630 83348630 83348629 83348629

After the program begins to run, press the "Pause" key stop the display output
(this stops the primary thread's I/O, but the secondary thread continues to run
in the background) and any other key to restart it.
*/
/*	Critical Section Objects

What if your main thread needed all elements of the array to processed prior to
reading? One solution is to use a critical section.

Critical section objects provide synchronization similar to that provided by
mutex objects, except critical section objects can be used only by the threads
of a single process. Event, mutex, and semaphore objects can also be used in
a single-process application, but critical section objects provide a slightly
faster, more efficient mechanism for mutual-exclusion synchronization. Like a
mutex object, a critical section object can be owned by only one thread at a
time, which makes it useful for protecting a shared resource from simultaneous
access. There is no guarantee about the order in which threads will obtain
ownership of the critical section, however, the system will be fair to all
threads.

  #include <windows.h>
  #include <process.h>
  #include <stdio.h>

  CRITICAL_SECTION cs;
  int a[ 5 ];

  void Thread( void* pParams )
  {
    int i, num = 0;

    while ( TRUE )
    {
       EnterCriticalSection( &cs );
       for ( i = 0; i < 5; i++ ) a[ i ] = num;
       LeaveCriticalSection( &cs );
       num++;
    }
  }

  int main( void )

  {
    InitializeCriticalSection( &cs );
    _beginthread( Thread, 0, NULL );

    while( TRUE )
    {
       EnterCriticalSection( &cs );
       printf( "%d %d %d %d %d\n",
               a[ 0 ], a[ 1 ], a[ 2 ],
               a[ 3 ], a[ 4 ] );
       LeaveCriticalSection( &cs );
    }
    return 0;
  }
*/
/*	Mutex Objects

A mutex object is a synchronization object whose state is set to signaled when
it is not owned by any thread, and non-signaled when it is owned. Only one
thread at a time can own a mutex object, whose name comes from the fact that
it is useful in coordinating mutually exclusive access to a shared resource.
For example, to prevent two threads from writing to shared memory at the same
time, each thread waits for ownership of a mutex object before executing the
code that accesses the memory. After writing to the shared memory, the thread
releases the mutex object.

Two or more processes can call CreateMutex to create the same named mutex.
The first process actually creates the mutex, and subsequent processes open a
handle to the existing mutex. This enables multiple processes to get handles of
the same mutex, while relieving the user of the responsibility of ensuring that
the creating process is started first. When using this technique, you should set
the bInitialOwner flag to FALSE; otherwise, it can be difficult to be certain
which process has initial ownership.

Multiple processes can have handles of the same mutex object, enabling use of
the object for interprocess synchronization. The following object-sharing
mechanisms are available:

* A child process created by the CreateProcess function can inherit a handle to
  a mutex object if the lpMutexAttributes parameter of CreateMutex enabled
  inheritance.
* A process can specify the mutex-object handle in a call to the DuplicateHandle
  function to create a duplicate handle that can be used by another process.
* A process can specify the name of a mutex object in a call to the OpenMutex or
  CreateMutex function.

Generally speaking, if you are synchronizing threads within the same process, a
critical section object is more efficient.

  #include <windows.h>
  #include <process.h>
  #include <stdio.h>

  HANDLE hMutex;
  int a[ 5 ];

  void Thread( void* pParams )
  {
     int i, num = 0;

     while ( TRUE )
     {
        WaitForSingleObject( hMutex, INFINITE );
        for ( i = 0; i < 5; i++ ) a[ i ] = num;
        ReleaseMutex( hMutex );
        num++;
     }
  }

  int main( void )
  {
     hMutex = CreateMutex( NULL, FALSE, NULL );
     _beginthread( Thread, 0, NULL );

     while( TRUE )

     {
        WaitForSingleObject( hMutex, INFINITE );
        printf( "%d %d %d %d %d\n",
                a[ 0 ], a[ 1 ], a[ 2 ],
                a[ 3 ], a[ 4 ] );
        ReleaseMutex( hMutex );
     }
     return 0;
  }
*/
/*	Event Objects

What if we want to force the secondary thread to run each time the primary
thread finishes printing the contents of the global array, so that the values
in each line of output is only incremented by one?

An event object is a synchronization object whose state can be explicitly set to
signaled by use of the SetEvent or PulseEvent function. Following are the two
types of event object.

Object 				Description

Manual-reset event 	An event object whose state remains signaled until it is
					explicitly reset to non-signaled by the ResetEvent function.
					While it is signaled, any number of waiting threads, or
					threads that subsequently specify the same event object in
					one of the wait functions, can be released.
Auto-reset event 	An event object whose state remains signaled until a single
					waiting thread is released, at which time the system
					automatically sets the state to non-signaled. If no threads
					are waiting, the event object's state remains signaled.

The event object is useful in sending a signal to a thread indicating that a
particular event has occurred. For example, in overlapped input and output, the
system sets a specified event object to the signaled state when the overlapped
operation has been completed. A single thread can specify different event
objects in several simultaneous overlapped operations, then use one of the
multiple-object wait functions to wait for the state of any one of the event
objects to be signaled.

A thread uses the CreateEvent function to create an event object. The creating
thread specifies the initial state of the object and whether it is a manual-reset
or auto-reset event object. The creating thread can also specify a name for the
event object. Threads in other processes can open a handle to an existing event
object by specifying its name in a call to the OpenEvent function. For additional
information about names for mutex, event, semaphore, and timer objects, see
Interprocess Synchronization.

A thread can use the PulseEvent function to set the state of an event object to
signaled and then reset it to non-signaled after releasing the appropriate
number of waiting threads. For a manual-reset event object, all waiting threads
are released. For an auto-reset event object, the function releases only a single
waiting thread, even if multiple threads are waiting. If no threads are waiting,
PulseEvent simply sets the state of the event object to non-signaled and returns.

  #include <windows.h>
  #include <process.h>
  #include <stdio.h>

  HANDLE hEvent1, hEvent2;
  int a[ 5 ];

  void Thread( void* pParams )
  {
     int i, num = 0;

     while ( TRUE )
     {
        WaitForSingleObject( hEvent2, INFINITE );
        for ( i = 0; i < 5; i++ ) a[ i ] = num;
        SetEvent( hEvent1 );
        num++;
     }
  }

  int main( void )
  {
     hEvent1 = CreateEvent( NULL, FALSE, TRUE, NULL );
     hEvent2 = CreateEvent( NULL, FALSE, FALSE, NULL );

     _beginthread( Thread, 0, NULL );

     while( TRUE )
     {
        WaitForSingleObject( hEvent1, INFINITE );
        printf( "%d %d %d %d %d\n",
                a[ 0 ], a[ 1 ], a[ 2 ],
                a[ 3 ], a[ 4 ] );
        SetEvent( hEvent2 );
     }
     return 0;
  }

Summary of Synchronization Objects

The MSDN News for July/August 1998 has a front page article on Synchronization
Objects. The following table is from that article:

Name 		Speed 	Cross process 	Resource counting 		Platforms
Critical	Fast 	No 				No (exclusive access) 	9x/NT/CE
Mutex 		Slow 	Yes 			No (exclusive access) 	9x/NT/CE
Semaphore 	Slow 	Yes 			Automatic 				9x/NT
Event 		Slow 	Yes 			Yes 					9x/NT/CE
Metered
Section 	Fast 	Yes 			Automatic 				9x/NT/CE
*/
/*	Return Values - WaitForSingleObject() - Win32

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
