#include "../lib/hw.h"
#include "../h/sys_regs.hpp"
#include "../h/memory.hpp"
#include "../h/interrupt.hpp"
#include "../h/thread.hpp"
#include "../h/scheduler.hpp"
#include "../h/print.hpp"

using namespace kernel;

extern void userMain();
extern void mod30();

void userWrapper(void *) {
	//while (true)
		//userMain();
	mod30();
}

int main() {
	printString("INITIALIZATION:\n", PRINT_INFO);
	memInit();
	Thread::Init();
	Scheduler::Init();
	Console::Init();
	interruptInit();

	printString("Creating user thread.\n", PRINT_INFO);
	Thread *userT = Thread::create(userWrapper, nullptr, nullptr, Thread::Mode::USER);

	printString("****************\n", PRINT_INFO);
	printString("USER MAIN START:\n\n", PRINT_INFO);
	interruptEnable();
	while (!userT->isFinished() || !Scheduler::isEmpty()) {
		interruptDisable();
		Console::handle_output();
		interruptEnable();
		Thread::yield();
	}
	interruptDisable();
	Console::Destroy();
	printString("\nUSER MAIN END.\n", PRINT_INFO);
	printString("****************\n", PRINT_INFO);

	printString("SHUTDOWN\n", PRINT_INFO);
	Scheduler::Destroy();
	Thread::Destroy();
	terminate();
	return 0;
}
