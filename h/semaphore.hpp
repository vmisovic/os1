#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "../h/thread.hpp"
#include "../h/queue.hpp"

namespace kernel {

class Semaphore {
public:
	Semaphore(unsigned init = 1) : val(init), blocked() {}

	int value() const { return val; }

	// ecall wrappers to use in SYSETM mode
	void aquire();
	void release();
protected:
	void wait();
	void signal();
	int timedWait(time_t timeout);
	int tryWait();
private:
	long int val;
	Queue<Thread> blocked;

	friend void interruptHandler(volatile Registers *saved);
	friend void ecallHandler(volatile Registers *saved);
};

}

#endif
