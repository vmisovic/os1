#include "../h/scheduler.hpp"

// staticka inicijalizacija objekta List
// fale neki simboli pa mora
// CXXFLAGS += -fno-use-cxa-atexit
List<Thread> Scheduler::waiting;

void Scheduler::put(Thread *ready) {
	waiting.insertBack(ready);
}

Thread* Scheduler::get() {
	return waiting.removeFront();
}
