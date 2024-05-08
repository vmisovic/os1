#include "../h/syscall_c.h"
#include "../h/ecall_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

void* mem_alloc(size_t size) {
	size_t blocks = (size + MEM_BLOCK_SIZE - 1) % MEM_BLOCK_SIZE;
	void *mem = 0;
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a1, %[blocks];"
		"li a0, ECODE;"
		"ecall;"
		"mv %[mem], a0;"
		: [mem]"=r"(mem)
		: [blocks]"r"(blocks), [ecode]"i"(MEM_ALLOC)
	);
	return mem;
}

int mem_free(void *ptr) {
	int ret = 0;
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a1, %[ptr];"
		"li a0, ECODE;"
		"ecall;"
		"mv %[ret], a0;"
		: [ret]"=r"(ret)
		: [ptr]"r"(ptr), [ecode]"i"(MEM_FREE)
	);
	return ret;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg) {
	void *stack_space = 0;//(uint8*)mem_alloc(DEFAULT_STACK_SIZE) + DEFAULT_STACK_SIZE; // leaks memory!!?
	int ret = 0;
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a4, %[stack_space];"
		"mv a3, %[arg];"
		"mv a2, %[start_routine];"
		"mv a1, %[handle];"
		"li a0, ECODE;"
		"ecall;"
		"mv %[ret], a0;"
		: [ret]"=r"(ret)
		: [handle]"r"(handle),
		  [start_routine]"r"(start_routine),
		  [arg]"r"(arg),
		  [stack_space]"r"(stack_space),
		  [ecode]"i"(THREAD_CREATE)
	);
	return ret;
}

int thread_exit() {
	int ret = 0;
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"li a0, ECODE;"
		"ecall;"
		"mv %[ret], a0;"
		: [ret]"=r"(ret)
		: [ecode]"i"(THREAD_EXIT)
	);
	return ret;
}

void thread_dispatch() {
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"li a0, ECODE;"
		"ecall;"
		:
		: [ecode]"i"(THREAD_DISPATCH)
	);
}

char getc() {
	char c = 0;
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"li a0, ECODE;"
		"ecall;"
		"mv %[c], a0;"
		: [c]"=r"(c)
		: [ecode]"i"(GETC)
	);
	return c;
}

void putc(char c) {
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a1, %[c];"
		"li a0, ECODE;"
		"ecall;"
		:
		: [c]"r"(c), [ecode]"i"(PUTC)
	);
}

#ifdef __cplusplus
}
#endif
