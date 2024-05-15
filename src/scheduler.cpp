#include "../h/scheduler.hpp"
#include "../h/print.hpp"

namespace kernel {

Queue<Thread> *Scheduler::waiting = nullptr;
Scheduler::sleepNode *Scheduler::sleepingHead= nullptr;

void Scheduler::koSpava() {
	if (!PRINT_SLEEPY) return;
	printString("Spavaju:\n");
	for (sleepNode *cur = sleepingHead; cur; cur = cur->next) {
		printString("\tt: ");
		printInt(cur->relTime);
		printString("\n");
	}
}

void Scheduler::Init() {
	waiting = new Queue<Thread>;
	sleepingHead = nullptr;
}

void Scheduler::Destroy() {
	delete waiting;
	while (sleepingHead) {
		sleepNode *del = sleepingHead;
		sleepingHead = sleepingHead->next;
		delete del;
	}
}

void Scheduler::put(Thread *ready) {
	waiting->insert(ready);
}

Thread* Scheduler::get() {
	return waiting->remove();
}

void Scheduler::putToSleep(Thread *sleepy, time_t t) {
	printString("pre uspavljivanja:\n", PRINT_SLEEPY);
	koSpava();
	if (!sleepingHead || t < sleepingHead->relTime) {
		sleepingHead = new sleepNode(sleepy, t, sleepingHead);
		return;
	}
	sleepNode *cur = sleepingHead;
	t -= sleepingHead->relTime;
	while (cur->next && cur->next->relTime <= t) {
		cur = cur->next;
		t -= cur->relTime;
	}
	cur->next = new sleepNode(sleepy, t, cur->next);
	printString("posle uspavljivanja:\n", PRINT_SLEEPY);
	koSpava();
}

void Scheduler::tick() {
	printString("TICK:\n", PRINT_SLEEPY);
	koSpava();
	if (sleepingHead)
		sleepingHead->relTime--;
	while (sleepingHead && sleepingHead->relTime == 0) {
		printString("budim\n", PRINT_SLEEPY);
		Scheduler::put(sleepingHead->thread);
		sleepNode *del = sleepingHead;
		sleepingHead = sleepingHead->next;
		delete del;
	}
}

}
