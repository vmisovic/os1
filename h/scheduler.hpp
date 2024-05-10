#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include "../lib/hw.h"
#include "../h/queue.hpp"

class Thread;

class Scheduler {
public:
	static void Init();
	static void Destroy();

	static void put(Thread *ready);
	static Thread* get();

	static void putToSleep(Thread *sleepy, time_t t);
	static void tick();
private:
	static Queue<Thread> *waiting;

	struct sleepNode {
		Thread *thread;
		time_t relTime;
		sleepNode *next;
		sleepNode(Thread *th, time_t rt, sleepNode *n = nullptr) :
			thread(th), relTime(rt), next(n) {}
	};
	static sleepNode *sleepingHead;
};

#endif
