#ifndef BUFFER_HPP
#define BUFFER_HPP

#include "../h/semaphore.hpp"

namespace kernel {

class Buffer {
public:
	Buffer(int _cap);
	~Buffer();

	void put(char val);
	char get();

	int getCnt();
private:
	int cap;
	int head;
	int tail;
	char *buffer;

	Semaphore spaceAvailable;
	Semaphore itemAvailable;
	Semaphore mutexHead;
	Semaphore mutexTail;
};

}

#endif

