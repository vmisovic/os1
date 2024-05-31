#include "../h/syscall_c.h"

sem_t sem;

static char digits[] = "0123456789ABCDEF";

void print_int(int xx, int base, int sgn)
{
	char buf[16];
	int i, neg;
	uint x;

	neg = 0;
	if(sgn && xx < 0){
		neg = 1;
		x = -xx;
	} else {
		x = xx;
	}

	i = 0;
	do{
		buf[i++] = digits[x % base];
	}while((x /= base) != 0);
	if(neg)
		buf[i++] = '-';

	while(--i >= 0)
		putc(buf[i]);
}

void f(void *arg)
{
	uint64 a = (uint64)arg;
	int i = 100;
	while (i > 0) {

		print_int(sem_timedwait(sem, 10 * a), 10, 1);
		//sem_wait(sem);
		print_int(a, 10, 0);
		putc('\t');
		print_int(i, 10, 0);
		putc('\n');

		sem_signal(sem);
		i--;
		thread_dispatch();
	}
}

__attribute__((noreturn)) void myUserMain()
{
	sem_open(&sem, 0);
	thread_t t1;
	thread_t t2;
	thread_create(&t1, f, (uint64*)1);
	thread_create(&t2, f, (uint64*)2);

	while (1) {
		thread_dispatch();
		//putc(getc());
	}
}
