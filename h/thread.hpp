#ifndef THREAD_CPP
#define THREAD_CPP

#include "../lib/hw.h"

namespace kernel {

struct Registers;
class Semaphore;

class Thread {
public:
	static void Init();
	static void Destroy();
	~Thread();

	enum Mode { SYSTEM, USER };

	static Thread *running;

	static Thread* create(
		void (*run_routine)(void*),
		void *args,
		void* sp,
		Mode m
	);
	bool isFinished() const { return finished; }
	Semaphore *waitingOnSemaphoe() { return waitingOn; }

	// ecall wrappers to use in SYSETM mode
	static void compleated();
	static void yield();
private:
	struct Context {
		uint64 ra;
		uint64 sp;
	};
	uint64 ret_val = 0;
	Semaphore *waitingOn = nullptr;

	void (*run_routine)(void*);
	void *args;
	void *stack;
	Context context;
	time_t timerCounter = 0;
	time_t sleepingTime = 0;
	bool finished = false;
	bool blocked = false;
	Mode mode;

	Thread();
	Thread(void (*run_routine)(void*), void *args, void* sp, Mode m);
	static void start_wrapper();
	static void set_priviledge(Mode m);
	static void switchContext(Context *oldC, Context *newC);
	static void exit();
	static void dispatch();
	static void putToSleep(time_t timeout);

	void block(Semaphore *owner, time_t timeout);
	void unblock();

	friend void interruptHandler(volatile Registers *saved);
	friend void userEcallHandler(volatile Registers *saved);
	friend void systemEcallHandler(volatile Registers *saved);
	friend class Semaphore;
};

}

#endif
