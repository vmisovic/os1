#include "../h/memory.hpp"
#include "../h/syscall_c.h"

// new int[len] requires some symbols
// CXXFLAGS += -fno-exceptions

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
