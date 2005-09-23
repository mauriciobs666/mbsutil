#include "Thread.h"

/*
Thread::Thread(void *(*funcao)(void*), void *parametro, bool iniciar)
{
	int flags=0;
	if(!iniciar)
		flags=CREATE_SUSPENDED;
		
	if(funcao==NULL)
		return;
	hnd=CreateThread(NULL,0,funcao,parametro,flags,&id);
}
*/
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

/*
Typecasting Functions in C/C++


This document covers the use of functions typecasts or typecasting functions ... It was annoying to figure out, but here it is.
Typecasting functions allows you to cast them to void* and use them as arguments to functions.


Some basic functions...

int plus(int a, int b) { return x+y; }
int minus(int a, int b) { return x-y; }


How do you call these functions one by one as part of a for loop?

Simple...

typedef int (*BINOP)(int, int);

void Test() {
  BINOP f[] = {plus, minus};
  const int n = sizeof(f) / sizeof(*f);

  for (int i = 0; i < n; i++) {
    printf ("%d\n", (*f[i])(7, 3));
  }
}


The important parts here are the typedef and the bit where we call the function.

The typedef
I've always been used to typedef's like:

typedef unsigned char byte;


Which mean: Create a new variable type called "byte" that is an "unsigned char".

Or in other words:

typedef how_to_make this_type;


A function typedef, works like:

typedef return_type (*SOME_NAME)(arguments);


The calling...

So our BINOP typedef above, declares a function that returns an int, and takes two int parameters.

BINOP f[] = {plus, minus};
(*f[0])(7, 3); // calls plus(7, 3)
(*f[1])(7, 3); // calls minus(7, 3)

const int n = sizeof(f) / sizeof(*f);

Is just a nice way of knowing how many elements is in the f array.

Or in other words:
const int size = the_size_of_the_list / the_size_of_the_first_element_in_the_list

Calling a void pointer...
To call a function based on a void pointer you need to typecast the void pointer to a void function pointer.
If only it were this simple...

void func(void);
void* ptr = func;
ptr();

But it isn't!
Again, typedef is needed.

typedef void (*VOID_FUNC)(void);

void Foo(void) {
  printf ("Foo was called...\n");
}

void Test(void* ptr) {
  VOID_FUNC func = (VOID_FUNC) ptr;
  (*func)();
}

void Example() {
  Test((void*) Foo);
}


Example is called, which calls Test with the argument Foo,
and then Test calls Foo and Foo prints "Foo was called..."
Passing Foo to Test sends Test a void pointer
We have to typecast it to a void pointer because some compilers don't do this for us...

we then declare a variable 'func' of type 'VOID_FUNC',
whose value is 'ptr' but typecast to the VOID_FUNC type.

And so with just a little effort, it's easy to take any old pointer and call it.

Try not to write ugly code with this though!

Just incase you wondered...
This document came into existance because I was trying to write a simple portable thread function.

void ExecuteThread(void func(void));
*/
