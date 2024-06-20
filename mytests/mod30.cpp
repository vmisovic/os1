#include "../h/syscall_cpp.hpp"
#include "../test/printing.hpp"
#include "../h/print.hpp"

static Semaphore *sem;

class myThread : public Thread {
public:
	void run() override {
		int i = 3;
		int id = getCurrentId();
		while (i > 0) {
			int ret = sem->timedWait(id);
			if (ret == 0) {
				printString("-> entered: ");
				printInt(id); printString("\n");
				printString("\t i = ");
				printInt(i); printString("\n");

				Thread::sleep(id);
				printString("<- exited: ");
				printInt(id);
				printString("\n");
				sem->signal();
				i--;
			} else if (ret == -2) {
				printString("**elapsed time: ");
				printInt(id);
				printString("\n");
			}
			Thread::dispatch();
		}
	}
};


void mod30() {
	sem = new Semaphore(5);
	myThread *niti[10];

	for (int i = 0; i < 10; i++) {
		niti[i] = new myThread();
		niti[i]->start();
	}
}
