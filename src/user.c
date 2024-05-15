#include "../h/syscall_c.h"

#ifdef __cplusplus
extern "C" {
#endif

static sem_t semafor;
static sem_t end;

void dete(void *arg) {
	putc('D');
	putc('E');
	putc('T');
	putc('E');
	putc('\n');


	putc('w');
	putc('a');
	putc('i');
	putc('t');
	putc('\n');
	sem_wait(semafor);
	putc('O');
	putc('K');
	putc('\n');
	sem_signal(end);
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

	sem_open(&semafor, 0);
	sem_open(&end, 0);

	int *deteArg = (int*)mem_alloc(sizeof(int));
	*deteArg = 3;
	thread_t nit;
	thread_create(&nit, dete, deteArg);

	putc('s');
	putc('l');
	putc('e');
	putc('e');
	putc('p');
	putc('\n');
	time_sleep(30);
	putc('s');
	putc('i');
	putc('g');
	putc('n');
	putc('a');
	putc('l');
	putc('\n');
	sem_signal(semafor);

	sem_wait(end);
	putc('e');
	putc('n');
	putc('d');
	putc('\n');
	sem_close(semafor);
	sem_close(end);
	mem_free(deteArg);
	thread_exit();
}

#ifdef __cplusplus
}


#endif
