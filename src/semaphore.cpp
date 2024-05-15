#include "../h/semaphore.hpp"
#include "../h/ecall_codes.h"
#include "../h/print.hpp"

namespace kernel {

void Semaphore::aquire() {
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a1, %[id];"
		"li a0, ECODE;"
		"ecall;"
		:
		: [id]"r"(this),
		  [ecode]"i"(SEM_WAIT)
	);
}

void Semaphore::release() {
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a1, %[id];"
		"li a0, ECODE;"
		"ecall;"
		:
		: [id]"r"(this),
		  [ecode]"i"(SEM_SIGNAL)
	);
}

void Semaphore::wait() {
	if (--val < 0) {
		printString("Blokiranje trenutne niti.\n", PRINT_SEMAPHORE);
		blocked.insert(Thread::running);
		Thread::running->block();
	}
}

void Semaphore::signal() {
	if (++val <= 0) {
		printString("Deblokiranje niti iz reda blocked.\n", PRINT_SEMAPHORE);
		Thread *t = blocked.remove();
		if (t != nullptr) t->unblock();
	}
}

int Semaphore::timedWait(time_t timeout) {
	if (tryWait() == 0)
		return 0;
	Thread::putToSleep(timeout);
	if (tryWait() == 0)
		return 0;
	printString("timedWait isteklo vreme.\n", PRINT_SEMAPHORE);
	return -2;
}

int Semaphore::tryWait() {
	if (val - 1 < 0)
		return 1;
	--val;
	return 0;
}

}
