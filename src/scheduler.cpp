#include "../h/scheduler.hpp"

Scheduler::waitNode *Scheduler::waitingHead = nullptr;
Scheduler::waitNode *Scheduler::waitingTail= nullptr;
Scheduler::sleepNode *Scheduler::sleepingHead= nullptr;

void Scheduler::put(Thread *ready) {
	waitNode *newNode = new waitNode(ready);
	if (waitingTail)
		waitingTail = waitingTail->next = newNode;
	else
		waitingHead = waitingTail = newNode;
}

Thread* Scheduler::get() {
	if (!waitingHead) return nullptr;
	waitNode *del = waitingHead;
	Thread *ret = del->thread;
	waitingHead = waitingHead->next;
	delete del;
	if (!waitingHead)
		waitingTail = nullptr;
	return ret;
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
