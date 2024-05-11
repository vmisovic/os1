#include "../h/semaphore.hpp"
#include "../h/ecall_codes.h"

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
	if (--val < 0) 
		Thread::running->block();
}

void Semaphore::signal() {
	if (++val <= 0) {
		Thread *t = blocked.remove();
		t->unblock();
	}
}

int Semaphore::timedWait(time_t timeout) {
	if (tryWait() == 0)
		return 0;
	Thread::putToSleep(timeout);
	if (tryWait() == 0)
		return 0;
	return -2;
}

int Semaphore::tryWait() {
	if (val - 1 < 0)
		return 1;
	--val;
	return 0;
}

}
