#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "../h/thread.hpp"
#include "../h/queue.hpp"

namespace kernel {

#define SEM_RET_OK	 0
#define SEM_RET_CLOSED	 1
#define SEM_RET_DEAD	-1
#define SEM_RET_TIMEOUT -2

class Semaphore {
public:
	Semaphore(unsigned init = 1) : val(init), blocked() {}
	~Semaphore();

	int value() const { return val; }
	void timedDeblock(Thread *t);

	// ecall wrappers to use in SYSETM mode
	void aquire();
	void release();
protected:
	void wait();
	void signal();
	void timedWait(time_t timeout);
	int tryWait();
private:
	long int val;
	Queue<Thread> blocked;

	friend void interruptHandler(volatile Registers *saved);
	friend void ecallHandler(volatile Registers *saved);
};

}

#endif
