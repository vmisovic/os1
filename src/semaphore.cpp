#include "../h/semaphore.hpp"
#include "../h/ecall_codes.h"
#include "../h/print.hpp"

namespace kernel {

int Semaphore::aquire() {
	int ret;
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a1, %[id];"
		"li a0, ECODE;"
		"ecall;"
		"mv %[ret], a0;"
		: [ret]"=r"(ret)
		: [id]"r"(this),
		  [ecode]"i"(SEM_WAIT)
	);
	return ret;
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
		Thread::running->block(this, 0);
	}
}

void Semaphore::signal() {
	if (++val <= 0) {
		Thread *t = blocked.remove();
		if (t != nullptr) {
			printString("Deblokiranje niti iz reda blocked.\n", PRINT_SEMAPHORE);
			t->ret_val = (uint64)SEM_RET_OK;
			t->unblock();
		}
	}
}

void Semaphore::timedWait(time_t timeout) {
	if (tryWait() == SEM_RET_OK) {
		Thread::running->ret_val = (uint64)SEM_RET_OK;
		return;
	}
	if (timeout == 0) {
		Thread::running->ret_val = (uint64)SEM_RET_TIMEOUT;
		return;
	}
	printString("Blokiranje trenutne niti.\n", PRINT_SEMAPHORE);
	--val;
	blocked.insert(Thread::running);
	Thread::running->block(this, timeout);
}

int Semaphore::tryWait() {
	if (val - 1 < 0)
		return SEM_RET_CLOSED;
	--val;
	return SEM_RET_OK;
}

void Semaphore::timedDeblock(Thread *t) {
	if (blocked.remove(t)) {
		printString("Deblokiranje: TIMEOUT\n", PRINT_SEMAPHORE);
		++val;
		t->ret_val = SEM_RET_TIMEOUT;
	}
}

Semaphore::~Semaphore() {
	Thread *t = blocked.remove();
	while (t != nullptr) {
		printString("Deblokiranje: SEM DEAD\n", PRINT_SEMAPHORE);
		t->ret_val = (uint64)SEM_RET_DEAD;
		t->unblock();
		t = blocked.remove();
	}
}

}
