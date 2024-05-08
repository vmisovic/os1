#ifndef SYSCALL_C
#define SYSCALL_C
// C API - syscall_c.h

#include "../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif

	void* mem_alloc(size_t size);
	int mem_free(void*);

	//class _thread;
	//typedef _thread* thread_t;
	//int thread_create(
	//	thread_t* handle,
	//	void(*start_routine)(void*),
	//	void* arg
	//);
	int thread_exit();
	void thread_dispatch();
 	 
	//const int EOF = -1; 
	char getc();
	void putc(char);


#ifdef __cplusplus
}
#endif

#endif
