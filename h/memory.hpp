#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "../lib/hw.h"

namespace kernel {

void memInit();

// approximate heap size: 128MB
// for blocks of size in range [16B, 2048B] NUM_OF_BLOCKS can fit in 3B or an signed integer.
typedef int blk_t;

void *blkAlloc(size_t blocks);
int blkFree(void *ptr);
void *memAlloc(size_t bytes);
int memFree(void *ptr);

}

#endif
