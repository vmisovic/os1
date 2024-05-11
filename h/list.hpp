#ifndef LIST_HPP
#define LIST_HPP

template<typename T>
class List {
private:
	struct Node {
		T* data;
		Node* next;
		Node(T *data, Node *next) : data(data), next(next) {}
	};
	Node* head;
	Node* tail;
public:
	List() : head(0), tail(0) {}
	List(const List<T> &) = delete;

	~List() {
		while (head != nullptr) {
			Node* temp = head;
			head = head->next;
			delete temp;
		}
	}

	void insertFront(T *value) {
		Node* newNode = new Node(value, head);
		head = newNode;
		if (!tail) head = tail;
	}

	void insertBack(T *value) {
		Node* newNode = new Node(value, 0);
		if (tail) {
			tail->next = newNode;
			tail = newNode;
		} else {
			head = tail = newNode;
		}
	}

	T* removeFront() {
		if (!head) return 0;
		Node* temp = head;
		T *data = temp->data;
		head = head->next;
		if (!head)
			tail = 0;
		delete temp;
		return data;
	}
};

#endif
