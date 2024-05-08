#ifndef SCHEDULER_CPP
#define SCHEDULER_CPP

#include "../h/list.hpp"
class Thread;

class Scheduler {
public:
	static void put(Thread *ready);
	static Thread* get();
private:
	static List<Thread> waiting;
};

#endif
