#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include "../h/list.hpp"
class Thread;

class Scheduler {
public:
	static void put(Thread *ready);
	static Thread* get();

	static void putToSleep(Thread *sleepy, time_t t);
	static void tick();
private:
	struct waitNode {
		Thread *thread;
		waitNode *next;
		waitNode(Thread *th, waitNode *n = nullptr) :
			thread(th), next(n) {}
	};
	static waitNode *waitingHead;
	static waitNode *waitingTail;

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
