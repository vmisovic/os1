#ifndef QUEUE_HPP
#define QUEUE_HPP

typedef unsigned long size_t;

#define DEFAULT_QUEUE_LEN 32
#include "../h/print.hpp"

template<typename T>
class Queue {
public:
	Queue(size_t len = DEFAULT_QUEUE_LEN) :
		len(len),
		fron(len),
		rear(len),
		arr(new T*[len])
	{}
	~Queue() { delete[] arr; }

	void insert(T *data);
	T* remove();

	void clear() { fron = rear = len; }
	bool isEmpty() const { return (fron == len); }
private:
	size_t len;
	size_t fron;
	size_t rear;
	T **arr;
};

template<typename T>
void Queue<T>::insert(T *data) {
	if (rear == len)
		fron = rear = 0;
	else {
		rear = (rear + 1) % len;
		if (rear == fron) {
			// resize
			T **newArr = new T*[len * 2];
			for (size_t i = 0; i < len; i++) {
				newArr[i] = arr[fron];
				fron = (fron + 1) % len;
			}
			fron = 0;
			rear = len;
			len = len * 2;
			delete[] arr;
			arr = newArr;
		}
	}
	arr[rear] = data;
}

template<typename T>
T* Queue<T>::remove() {
	if (fron == len)
		return nullptr;
	T* ret = arr[fron];
	if (fron == rear)
		clear();
	else
		fron = (fron + 1) % len;
	return ret;
}

#endif
