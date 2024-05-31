#ifndef HASHMAP_HPP
#define HASHMAP_HPP

typedef unsigned long size_t;

#define DEFAULT_HASHMAP_SIZE 32

template<typename T>
class HashMap {
public:
	explicit HashMap(size_t size = DEFAULT_HASHMAP_SIZE) :
		table(new Node*[size]()),
		tableSize(size),
		count(0) 
	{}
	~HashMap() { clear(); delete[] table; }

	void insert(size_t key, T value);
	T* get(size_t key) const;
	bool remove(size_t key);
	void clear();
	bool getCnt() const { return count; }
private:
	struct Node {
		size_t key;
		T value;
		Node* next;
		Node(size_t k, T v) : key(k), value(v), next(nullptr) {}
	};

	Node** table;
	size_t tableSize;
	size_t count;
};

template<typename T>
void HashMap<T>::insert(size_t key, T value) {
	size_t index = key % tableSize;
	Node* newNode = new Node(key, value);
	if (!table[index]) {
		table[index] = newNode;
	} else {
		Node* current = table[index];
		while (current->next) {
			if (current->key == key) {
				current->value = value;
				return;
			}
			current = current->next;
		}
		current->next = newNode;
	}
	++count;
}

template<typename T>
T* HashMap<T>::get(size_t key) const {
	size_t index = key % tableSize;
	Node* current = table[index];
	while (current) {
		if (current->key == key)
			return &current->value;
		current = current->next;
	}
	return nullptr;
}

template<typename T>
bool HashMap<T>::remove(size_t key) {
	size_t index = key % tableSize;
	Node* current = table[index];
	Node* prev = nullptr;
	while (current) {
		if (current->key == key) {
			if (prev)
				prev->next = current->next;
			else
				table[index] = current->next;
			delete current;
			--count;
			return true;
		}
		prev = current;
		current = current->next;
	}
	return false;
}

template<typename T>
void HashMap<T>::clear() {
	for (size_t i = 0; i < tableSize; ++i) {
		Node* current = table[i];
		while (current) {
			Node* next = current->next;
			delete current;
			current = next;
		}
		table[i] = nullptr;
	}
	count = 0;
}

#endif
