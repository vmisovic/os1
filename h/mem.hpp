#ifndef MEM_HPP
#define MEM_HPP

#include "../lib/hw.h"

void memInit();
void *memAlloc(size_t bytes);
int memFree(void *ptr);

// approximate heap size: 128MB
// for blocks of size in range [16B, 2048B] NUM_OF_BLOCKS can fit in 3B or an signed integer.
typedef int blk_t;
void *blkAlloc(size_t blocks);
int blkFree(void *ptr);

#endif
