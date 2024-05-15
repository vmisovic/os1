#include "../lib/hw.h"
#include "../h/sys_regs.hpp"
#include "../h/memory.hpp"
#include "../h/interrupt.hpp"
#include "../h/thread.hpp"
#include "../h/scheduler.hpp"
#include "../h/print.hpp"

using namespace kernel;

extern "C" void userMain(void *args);

int main() {
	printString("INITIALIZATION:\n", PRINT_INFO);
	memInit();
	interruptInit();
	Thread::Init();
	Scheduler::Init();
	printString("Creating user thread.\n", PRINT_INFO);
	int *arg = new int;
	*arg = 9;
	Thread *userT = Thread::create(userMain, arg, nullptr, Thread::Mode::USER);
	interruptEnable();

	printString("****************\n", PRINT_INFO);
	printString("USER MAIN START:\n\n", PRINT_INFO);
	while (!userT->isFinished()) {
		Thread::yield();
	}
	printString("\nUSER MAIN END.\n", PRINT_INFO);
	printString("****************\n", PRINT_INFO);

	printString("SHUTDOWN\n", PRINT_INFO);
	Scheduler::Destroy();
	Thread::Destroy();
	terminate();
	return 0;
}
