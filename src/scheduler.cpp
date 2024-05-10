#include "../h/scheduler.hpp"

Queue<Thread> *Scheduler::waiting = nullptr;
Scheduler::sleepNode *Scheduler::sleepingHead= nullptr;

void Scheduler::Init() {
	waiting = new Queue<Thread>;
	sleepingHead = nullptr;
}

void Scheduler::Destroy() {
	delete waiting;
}

void Scheduler::put(Thread *ready) {
	printString("waiting address:\n");
	printHex((uint64)&waiting);
	waiting->insert(ready);
}

Thread* Scheduler::get() {
	return waiting->remove();
}

void Scheduler::putToSleep(Thread *sleepy, time_t t) {
	if (sleepingHead == nullptr) {
		sleepingHead = new sleepNode(sleepy, t);
		return;
	}
	sleepNode *cur = sleepingHead;
	while (cur->next && cur->next->relTime <= t) {
		cur = cur->next;
		t -= cur->relTime;
	}
	cur->next = new sleepNode(sleepy, t, cur->next);
}

void Scheduler::tick() {
	while (sleepingHead && sleepingHead->relTime == 0) {
		Scheduler::put(sleepingHead->thread);
		sleepNode *del = sleepingHead;
		sleepingHead = sleepingHead->next;
		delete del;
	}
}
