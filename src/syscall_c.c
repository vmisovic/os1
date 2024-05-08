#include "../h/syscall_c.h"
#include "../h/ecall_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

void* mem_alloc(size_t size) {
	void *mem = 0;
	__asm__ __volatile__ (
		".equ ECODE, %[ecode]\n"
		"mv a1, %[size];"
		"li a0, ECODE;"
		"ecall;"
		"mv %[mem], a0;"
		: [mem]"=r"(mem)
		: [size]"r"(size), [ecode]"i"(MEM_ALLOC)
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
