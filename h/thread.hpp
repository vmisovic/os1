#ifndef THREAD_CPP
#define THREAD_CPP

#include "../lib/hw.h"

struct Registers;

class Thread {
public:
	enum Mode { SYSTEM, USER };

	static Thread *running;
	time_t timerCounter = 0;

	static Thread* create(
		void (*run_routine)(void*),
		void *args,
		void* sp,
		Mode m
	);
	static void compleated();
	static void yield();

	bool isFinished() { return finished; }

private:
	struct Context {
		uint64 ra;
		uint64 sp;
		uint64 sepc;
		uint64 sstatus;
	};
	static Thread initialThread;
	
	void (*run_routine)(void*);
	void *args;
	char *stack;
	Context context;
	bool finished = false;
	Mode mode;

	Thread();
	Thread(void (*run_routine)(void*), void *args, void* sp, Mode m);
	~Thread();
	static void start_wrapper();
	static void set_priviledge(Mode m);
	static void switchContext(Context *oldC, Context *newC);
	static void dispatch();

	friend void interruptHandler(Registers *saved);
	friend void userEcallHandler(Registers *saved);
	friend void systemEcallHandler(Registers *saved);
};

#endif
