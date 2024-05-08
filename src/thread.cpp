#include "../h/thread.hpp"
#include "../h/mem.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys_regs.hpp"
#include "../h/ecall_codes.h"

// static allocation for initial thread, it will be automaticly dealocated
Thread Thread::initialThread;
Thread *Thread::running = &initialThread;

Thread::Thread(void (*run_routine)(void*), void *args, Mode mode) :
	run_routine(run_routine),
	args(args),
	stack((char*)memAlloc(DEFAULT_STACK_SIZE)),
	context({
		(uint64)start_wrapper,
		(uint64)(stack + DEFAULT_STACK_SIZE)
	}),
	mode(mode)
{
	printString("START_WRAPPER ADDR: ");
	printHex((uint64)start_wrapper);
	printString("\n");

	printString("CONTEXT ra: ");
	printHex((uint64)context.ra);
	printString("\n");

	for (size_t i = 0; i < DEFAULT_STACK_SIZE / sizeof(uint64); i++)
		*((uint64*)stack + i) = (uint64)args;
	Scheduler::put(this);
}

Thread::Thread() :
	run_routine(nullptr),
	args(nullptr),
	stack(nullptr),
	context({0}),
	mode(SYSTEM)
{
	printString("running: ");
	printHex((uint64)this);
	printString(" initalized.\n");
}

Thread::~Thread() {
	memFree(stack);
}

void Thread::compleated() {
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"li a0, ECODE;"
		"ecall;"
		:
		: [ecode]"i"(THREAD_EXIT)
	);
}

void Thread::yield() {
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"li a0, ECODE;"
		"ecall;"
		:
		: [ecode]"i"(THREAD_DISPATCH)
	);
}

void Thread::start_wrapper() {
	// when newly created thread gets cpu (it becomes running)
	// set priviledge mode for next instruction
	set_priviledge(running->mode);
	// it starts run method with args in specified mode
	running->run_routine(running->args);
	compleated();
}

void Thread::set_priviledge(Mode m) {
	uint64 sstatus = read_sstatus();
	switch(m) {
	case SYSTEM:
		sstatus |= SPP;
		break;
	case USER:
		sstatus &= ~SPP;
		break;
	}
	write_sstatus(sstatus);

	__asm__ __volatile__ (
		"csrw sepc, ra;"
		"sret;"
	);
}

/*void Thread::switchContext(Context *oldC, Context *newC) {
	oldC->sepc = read_sepc();
	oldC->sstatus = read_sstatus();
	__asm__ __volatile__ (
		"sd ra, 0 * 8(%[oldC]);"
		"sd sp, 1 * 8(%[oldC]);"
		"ld ra, 0 * 8(%[newC]);"
		"ld sp, 1 * 8(%[newC]);"
		: : [oldC]"r"(oldC), [newC]"r"(newC)
	);
	write_sstatus(newC->sstatus);
	write_sepc(newC->sepc);
}*/

void Thread::dispatch() {
	Thread *oldRunning = running;

	if (!oldRunning->finished)
		Scheduler::put(oldRunning);
	running = Scheduler::get();
	running->timerCounter = 0;

	switchContext(&oldRunning->context, &running->context);
}
