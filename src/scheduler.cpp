#include "../h/scheduler.hpp"
#include "../h/print.hpp"
#include "../h/thread.hpp"

namespace kernel {

Queue<Thread> *Scheduler::waiting = nullptr;
Scheduler::sleepNode *Scheduler::sleepingHead= nullptr;

void Scheduler::koSpava() {
	if (!PRINT_SLEEPY) return;
	printString("Spavaju:\n");
	for (sleepNode *cur = sleepingHead; cur; cur = cur->next) {
		printString("\trelTime: ");
		printInt(cur->relTime);
		printString("\tthread: ");
		printHex((uint64)cur->thread);
		printString("\n");
	}
}

void Scheduler::Init() {
	printString("Scheduler Init, new queue waiting.\n", PRINT_INFO);
	waiting = new Queue<Thread>;
	sleepingHead = nullptr;
}

void Scheduler::Destroy() {
	printString("Scheduler destroy waiting and sleeping.\n", PRINT_INFO);
	delete waiting;
	while (sleepingHead) {
		sleepNode *del = sleepingHead;
		delete del->thread;
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
		Thread *t = sleepingHead->thread;
		Semaphore *waitingOn = t->waitingOnSemaphoe();
		if (waitingOn != nullptr)
			waitingOn->timedDeblock(t);
		Scheduler::put(t);
		sleepNode *del = sleepingHead;
		sleepingHead = sleepingHead->next;
		delete del;
	}
}

void Scheduler::wakeUp(Thread *sleeping) {
	printString("Removing sleepy:\n", PRINT_SLEEPY);
	koSpava();
	if (sleepingHead == nullptr) return;

	if (sleepingHead->thread == sleeping) {
		printString("budim\n", PRINT_SLEEPY);
		sleepNode *del = sleepingHead;
		Scheduler::put(del->thread);
		sleepingHead = del->next;
		delete del;
		return;
	}
	sleepNode *cur = sleepingHead;
	while (cur->next && cur->next->thread != sleeping)
		cur = cur->next;

	if (cur->next) {
		printString("budim\n", PRINT_SLEEPY);
		sleepNode *del = cur->next;
		Scheduler::put(del->thread);
		if (del->next)
			del->next->relTime += del->relTime;
		cur->next = del->next;
		delete del;
	}
}

}
