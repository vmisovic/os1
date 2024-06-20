#ifndef SYSCALL_C
#define SYSCALL_C
// C API - syscall_c.h

#include "../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef void* thread_t;
	typedef size_t sem_t;

	void* mem_alloc(size_t size);
	int mem_free(void*);

	int thread_create(
		thread_t* handle,
		void(*start_routine)(void*),
		void* arg
	);
	int thread_exit();
	void thread_dispatch();
	int getCurrentId();
 	 
	int sem_open(sem_t *handle, unsigned init);
	int sem_close(sem_t handle);
	int sem_wait(sem_t id);
	int sem_signal(sem_t id);
	int sem_timedwait(sem_t id, time_t timeout);
	int sem_trywait(sem_t id);

	int time_sleep (time_t timeout);

	//const int EOF = -1; 
	char getc();
	void putc(char);
#ifdef __cplusplus
}
#endif

#endif
