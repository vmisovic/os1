#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include "../lib/hw.h"
#include "../h/queue.hpp"

namespace kernel {

class Thread;

class Scheduler {
public:
	static void Init();
	static void Destroy();

	static void put(Thread *ready);
	static Thread* get();

	static void putToSleep(Thread *sleepy, time_t t);
	static void wakeUp(Thread *sleeping);
	static void tick();
	static bool isEmpty() { return waiting->isEmpty() && !sleepingHead; }
private:
	static Queue<Thread> *waiting;

	struct sleepNode {
		Thread *thread;
		time_t relTime;
		sleepNode *next;
		sleepNode(Thread *th, time_t rt, sleepNode *n = nullptr) :
			thread(th), relTime(rt), next(n)
		{ if (next) next->relTime -= rt; }
	};
	static sleepNode *sleepingHead;

	static void koSpava();
};

}

#endif
