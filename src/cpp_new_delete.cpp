#include "../h/memory.hpp"
#include "../h/syscall_c.h"

extern int currentMode;
// 0 - SYSTEM mode
// 1 - USER mode

void *(*allocf[])(size_t) = { kernel::memAlloc, mem_alloc };
int (*freef[])(void*) = { kernel::memFree, mem_free };

void *operator new(size_t size) {
	return allocf[currentMode](size);
}

void *operator new[](size_t size) {
	return allocf[currentMode](size);
}

void operator delete(void *ptr) noexcept {
	freef[currentMode](ptr);
}

void operator delete[](void *ptr) noexcept {
	freef[currentMode](ptr);
}

// new int[len] requires symbol __cxa_throw_bad_array_new_length
// in Makefile add line: CXXFLAGS += -fno-exceptions
#ifdef __EXCEPTIONS
	// if -fno-exceptions flag is not set, define symbol:
	extern "C" void __cxa_throw_bad_array_new_length() {}
#endif
