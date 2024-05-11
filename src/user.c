#include "../h/syscall_c.h"

#ifdef __cplusplus
extern "C" {
#endif

sem_t semafor;

void dete(void *arg) {
	putc('D');
	putc('E');
	putc('T');
	putc('E');
	int n = *(int*)arg;
	thread_dispatch();
	while (n--) {
		while (sem_trywait(semafor) == 0) {
			putc('D');
		}
		sem_signal(semafor);
	}
}

void userMain(void *args) {
	int n = *(int*)args;
	putc((char)n+'0');
	putc('u');
	putc('s');
	putc('e');
	putc('r');
	putc(' ');
	putc('m');
	putc('a');
	putc('i');
	putc('n');
	putc('\n');
	int *deteArg = (int*)mem_alloc(sizeof(int));
	*deteArg = 3;
	thread_t nit;
	thread_create(&nit, dete, deteArg);
	sem_open(&semafor, 2);
	thread_dispatch();
	while (n--) {
		while (sem_trywait(semafor) == 0) {
			putc('U');
		}
		sem_signal(semafor);
		sem_signal(semafor);
		thread_dispatch();
	}
	mem_free(deteArg);
	thread_exit();
}

#ifdef __cplusplus
}


#endif
