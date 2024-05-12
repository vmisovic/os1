#ifndef SYSCALL_CPP
#define SYSCALL_CPP

#include "../h/syscall_c.h"

class Thread {
public:
	Thread(void (*body)(void*), void *arg) : body(body), arg(arg) {}
	virtual ~Thread();
	int start() { return thread_create(&myHandle, body, arg); }

	static void dispatch() { thread_dispatch(); }
	static int sleep(time_t timeout) { return time_sleep(timeout); }
protected:
	Thread() : body(runWrapper), arg(this) {}
	virtual void run() {}
private:
	thread_t myHandle = 0;
	void (*body)(void*);
	void *arg;

	static void runWrapper(void *arg) {
		Thread *thread = (Thread*)arg;
		thread->run();
	}
};


class Semaphore {
public:
	Semaphore(unsigned init = 1) { sem_open(&myHandle, init); }
	virtual ~Semaphore() { sem_close(myHandle); }

	int wait() { return sem_wait(myHandle); }
	int signal() { return sem_signal(myHandle); }
	int timedWait(time_t t) { return sem_timedwait(myHandle, t); }
	int tryWait() { return sem_trywait(myHandle); }
private:
	sem_t myHandle;
};

class PeriodicThread : public Thread {
public:
	void terminate() { terminated = true; }
protected:
	PeriodicThread(time_t period) :
		Thread(periodicWarapper, this),
		period(period) {}
	virtual void periodicActivation() {}
private:
	time_t period;
	bool terminated = false;

	static void periodicWarapper(void *arg) {
		PeriodicThread *pt = (PeriodicThread*)arg;
		while (!pt->terminated) {
			pt->periodicActivation();
			time_sleep(pt->period);
		}
	}
};

class Console {
public:
	static char getc() { return ::getc(); }
	static void putc(char c) { ::putc(c); }
};

#endif
