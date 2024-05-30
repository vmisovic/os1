#ifndef QUEUE_HPP
#define QUEUE_HPP

typedef unsigned long size_t;

#define DEFAULT_QUEUE_LEN 32

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
	T* remove(T *data);

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
	if (isEmpty())
		return nullptr;
	T* ret = arr[fron];
	if (fron == rear)
		clear();
	else
		fron = (fron + 1) % len;
	return ret;
}


template<typename T>
T* Queue<T>::remove(T *data) {
	if (isEmpty())
		return nullptr;
	size_t current = fron;
	while (current != rear) {
		if (arr[current] == data) {
			// shift elements
			size_t next = (current + 1) % len;
			while (next != rear) {
				arr[current] = arr[next];
				current = next;
				next = (next + 1) % len;
			}
			rear = (rear - 1 + len) % len;
			return data;
		}
		current = (current + 1) % len;
	}
	return nullptr;
}

#endif
