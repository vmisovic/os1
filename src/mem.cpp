#include "../h/mem.hpp"

// align last block to the MEM_BLOCK_SIZE to the end of the heap
static const uint8 *BLOCKS_END = (uint8*)(((size_t)HEAP_END_ADDR / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE);
// calculate number of blocks
static const blk_t NUM_OF_BLOCKS = (blk_t)(BLOCKS_END - (uint8*)HEAP_START_ADDR) / (MEM_BLOCK_SIZE + sizeof(blk_t));
static blk_t free_blocks_count = NUM_OF_BLOCKS;
// calculate address of the first block
static const uint8 *BLOCKS_START = BLOCKS_END - NUM_OF_BLOCKS * MEM_BLOCK_SIZE;

static blk_t *blockDesc; 

void memInit() {
	blockDesc = (blk_t*)HEAP_START_ADDR;
	blockDesc[0] = NUM_OF_BLOCKS;
	for (blk_t i = 1; i < NUM_OF_BLOCKS; i++)
		blockDesc[i] = 0;
}

blk_t next(blk_t index) {
	blk_t increment = blockDesc[index];
	increment = (increment >= 0)? increment : -increment;
	return index + increment;
}

void *blkAlloc(size_t blocks) {
	if ((size_t)free_blocks_count < blocks)
		return nullptr;
	blk_t count = (blk_t)blocks;
	for (blk_t i = 0; i < NUM_OF_BLOCKS; i = next(i)) {
		if (count <= blockDesc[i]) {
			blockDesc[i] = blockDesc[i] - count;
			i = next(i);
			blockDesc[i] = -count;
			free_blocks_count -= count;
			return (void*)(BLOCKS_START + i * MEM_BLOCK_SIZE);
		}
	}
	return nullptr;
}

int blkFree(void *ptr) {
	if ((uint8*)ptr < BLOCKS_START || (uint8*)ptr >= BLOCKS_END || (size_t)ptr % MEM_BLOCK_SIZE != 0)
		return -1;
	blk_t index = (blk_t)((uint8*)ptr - BLOCKS_START) / MEM_BLOCK_SIZE;
	if (blockDesc[index] >= 0)
		return -2;
	blockDesc[index] *= -1;
	// merge if next segemnt is also free
	blk_t nextIndex = next(index); 
	if (blockDesc[nextIndex] > 0) {
		blockDesc[index] += blockDesc[nextIndex];
		blockDesc[nextIndex] = 0;
	}
	// merge if prev segemnt is also free
	blk_t prevIndex = 0;
	while (next(prevIndex) < index)
		prevIndex = next(prevIndex);
	if (blockDesc[prevIndex] > 0) {
		blockDesc[prevIndex] += blockDesc[index];
		blockDesc[index] = 0;
	}
	return 0;
}
