#include "../lib/hw.h"
#include "../h/console.hpp"
#include "../h/memory.hpp"
#include "../h/print.hpp"
#include "../h/sys_regs.hpp"
//#include <stddef.h>
#include "../h/thread.hpp"
#include "../h/interrupt.hpp"
#include "../h/scheduler.hpp"

using namespace kernel;

#define MNB(i) (1ull << (i))

extern "C" void userMain(void *args);

void kurac(void *) {
	int i = 10;
	while (i--) {
		printString("kurac\n");
		Thread::yield();
	}
}

#include "../h/syscall_c.h"
void newTest(void *args) {
	int n = *(int*)args;
	int *niz = new int[n];
	for (int i = 0; i < n; i++) {
		niz[i] = i;
		putc((char)i+'0');
		thread_dispatch();
	}
	delete[] niz;
}

int main() {
	printString("Init()");

	memInit();
	Thread::Init();
	Scheduler::Init();

	printString("start: new thread\n");
	int *arg = new int;
	*arg = 9;
	//void *sp = (uint8*)memAlloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE;
	Thread *userT = Thread::create(userMain, arg, nullptr, Thread::Mode::USER);
	//Thread *kernT = new Thread(kurac, nullptr, nullptr, Thread::Mode::SYSTEM);
	printString("end: new thread\n");

	interruptInit();

	printString("while loop\n");
	while (!userT->isFinished()){//} || !kernT->isFinished()) {
		printString("main dosao na red, al ceka.\n");
		//read_sip();
		Thread::yield();
	}
	
	printString("delete int");
	delete arg;
	printString("delete threads\n");
	//delete kernT;

	Scheduler::Destroy();
	Thread::Destroy();

	printString("KRAJ!!!\n");
	terminate();
	return 0;
}
