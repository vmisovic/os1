#include "../h/thread.hpp"
#include "../h/memory.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys_regs.hpp"
#include "../h/ecall_codes.h"
#include "../h/print.hpp"

Thread *Thread::running = nullptr;

void Thread::Init() {
	running = new Thread();
}

void Thread::Destroy() {
	delete running;
}

Thread* Thread::create(void (*run_routine)(void*), void *args, void* sp, Mode m) {
	return new Thread(run_routine, args, sp, m);
}

Thread::Thread(void (*run_routine)(void*), void *args, void* sp, Mode mode) :
	run_routine(run_routine),
	args(args),
	stack((sp == nullptr)? memAlloc(DEFAULT_STACK_SIZE) : nullptr),
	context({
		(uint64)start_wrapper,
		(uint64)((sp == 0)? ((uint8*)stack + DEFAULT_STACK_SIZE) : sp)
	}),
	mode(mode)
{
	printString("\nSTART_WRAPPER ADDR: ");
	printHex((uint64)start_wrapper);
	printString("\nSTACK");
	printHex((uint64)stack);
	printString("\nCONTEXT ra: ");
	printHex((uint64)context.ra);
	printString("\nCONTEXT sp: ");
	printHex((uint64)context.sp);
	printString("\n");

	if (stack) {
		for (size_t i = 0; i < DEFAULT_STACK_SIZE / sizeof(uint64); i++)
			*((uint64*)stack + i) = (uint64)args;
	}
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
	if (stack) {
		printString("Brisem stek koji sam alocirao");
		memFree(stack);
	}
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
void Thread::block() {
	blocked = true;
	dispatch();
}

void Thread::unblock() {
	blocked = false;
	Scheduler::put(this);
}

void Thread::exit() {
	Thread::running->finished = true;
	delete Thread::running;
	Thread::running = nullptr;
	Thread::dispatch();
}

void Thread::dispatch() {
	Thread *oldRunning = running;

	if (oldRunning && !oldRunning->finished && !oldRunning->blocked) {
		if (oldRunning->sleepingTime > 0)
			Scheduler::putToSleep(oldRunning, oldRunning->sleepingTime);
		else
			Scheduler::put(oldRunning);
	}
	running = Scheduler::get();
	running->timerCounter = 0;
	running->sleepingTime = 0;

	Context *oldContext = (oldRunning)? &oldRunning->context : nullptr;
	switchContext(oldContext, &running->context);
}

void Thread::putToSleep(time_t timeout) {
	running->sleepingTime = timeout;
	dispatch();
}
