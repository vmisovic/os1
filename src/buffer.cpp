#include "../h/buffer.hpp"

namespace kernel {

Buffer::Buffer(int _cap) :
	cap(_cap + 1),
	head(0), 
	tail(0),
	spaceAvailable(_cap),
	itemAvailable(0),
	mutexHead(1),
	mutexTail(1)
{
	buffer = new char[cap];
}

Buffer::~Buffer() {
	delete buffer;
}

void Buffer::put(char val) {
	spaceAvailable.aquire();

	mutexTail.aquire();
	buffer[tail] = val;
	tail = (tail + 1) % cap;
	mutexTail.release();

	itemAvailable.release();
}

char Buffer::get() {
	itemAvailable.aquire();

	mutexHead.aquire();

	int ret = buffer[head];
	head = (head + 1) % cap;
	mutexHead.release();

	spaceAvailable.release();

	return ret;
}

int Buffer::getCnt() {
	int ret;

	mutexHead.aquire();
	mutexTail.aquire();

	if (tail >= head)
		ret = tail - head;
	else
		ret = cap - head + tail;

	mutexTail.release();
	mutexHead.release();

	return ret;
}

}
