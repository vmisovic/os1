#include "../h/thread.hpp"
#include "../h/memory.hpp"
#include "../h/scheduler.hpp"
#include "../h/sys_regs.hpp"
#include "../h/ecall_codes.h"
#include "../h/print.hpp"

extern kernel::Thread::Mode currentMode;

namespace kernel {

Thread *Thread::running = nullptr;

void Thread::Init() {
	running = new Thread();
	printString("Initial running thread: ", PRINT_INFO);
	printHex((uint64)running, PRINT_INFO);
	printString("\n", PRINT_INFO);
}

void Thread::Destroy() {
	printString("Destroying initial thread.\n", PRINT_INFO);
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
	printString("New thread created:\n", PRINT_THREAD);
	printString("STACK:\t", PRINT_THREAD);
	printHex((uint64)stack, PRINT_THREAD);
	printString("\nsp:\t", PRINT_THREAD);
	printHex((uint64)context.sp, PRINT_THREAD);
	printString("\nMode (0-SYSTEM, 1-USER): ", PRINT_THREAD);
	printInt(mode, PRINT_THREAD);
	printString("\n", PRINT_THREAD);

	Scheduler::put(this);
}

Thread::Thread() :
	run_routine(nullptr),
	args(nullptr),
	stack(nullptr),
	context({0}),
	mode(SYSTEM)
{}

Thread::~Thread() {
	printString("Thread destructor:\n", PRINT_THREAD);
	if (stack) {
		printString("Brisem alocirani stek:\n", PRINT_THREAD);
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
	currentMode = m;

	uint64 sstatus = read_sstatus();
	switch(m) {
	case SYSTEM:
		sstatus |= SPP;
		break;
	case USER:
		sstatus &= ~SPP;
		break;
	}
	sstatus |= SPIE;
	write_sstatus(sstatus);

	__asm__ __volatile__ (
		"csrw sepc, ra;"
		"sret;"
	);
}

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
	if (running == nullptr) {
		printString("running je null\n");
		printString("KERNEL PANIC\n");
		terminate();
	}
	running->timerCounter = 0;
	running->sleepingTime = 0;

	Context *oldContext = (oldRunning)? &oldRunning->context : nullptr;
	switchContext(oldContext, &running->context);
}

void Thread::putToSleep(time_t timeout) {
	running->sleepingTime = timeout;
	dispatch();
}

}
