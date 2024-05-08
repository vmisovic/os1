#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "../lib/hw.h"

void *__mem_alloc(size_t bytes);
int __mem_free(void *ptr);
void *__mem_cpy(void* to, void *from, size_t len);

#endif
