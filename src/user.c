#include "../h/syscall_c.h"

#ifdef __cplusplus
extern "C" {
#endif

void dete(void *arg) {
	putc('D');
	putc('E');
	putc('T');
	putc('E');
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
	thread_t nit;
	thread_create(&nit, dete, 0);
	thread_dispatch();
	//int *niz = mem_alloc(sizeof(int)*n);
	//for (int i = 0; i < n; i++) {
	//	niz[i] = i;
	//	putc((char)i+'0');
	//	thread_dispatch();
	//}
	//mem_free(niz);
	thread_exit();
}

#ifdef __cplusplus
}


#endif
