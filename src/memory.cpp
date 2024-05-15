#include "../h/memory.hpp"
#include "../h/print.hpp"

namespace kernel {

static uint8 *BLOCKS_START;
static uint8 *BLOCKS_END;

static blk_t NUM_OF_BLOCKS;
static blk_t free_blocks_count;

static blk_t *blockDesc;

void memInit() {
	printString("Heap memory initialization\n", PRINT_INFO);
	// align last block to the MEM_BLOCK_SIZE to the end of the heap
	BLOCKS_END = (uint8*)(((size_t)HEAP_END_ADDR / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE);
	// calculate number of blocks
	NUM_OF_BLOCKS = (blk_t)(BLOCKS_END - (uint8*)HEAP_START_ADDR) / (MEM_BLOCK_SIZE + sizeof(blk_t));
	// calculate address of the first block
	BLOCKS_START = BLOCKS_END - NUM_OF_BLOCKS * MEM_BLOCK_SIZE;
	free_blocks_count = NUM_OF_BLOCKS;

	printString("HEAP_START_ADDR:", PRINT_INFO);
	printHex((uint64)HEAP_START_ADDR, PRINT_INFO);
	printString("\nHEAP_END_ADDR:\t", PRINT_INFO);
	printHex((uint64)HEAP_END_ADDR, PRINT_INFO);
	printString("\nBLOCKS_START:\t", PRINT_INFO);
	printHex((uint64)BLOCKS_START, PRINT_INFO);
	printString("\nBLOCKS_END:\t", PRINT_INFO);
	printHex((uint64)BLOCKS_END, PRINT_INFO);
	printString("\nNUM_OF_BLOCKS:\t", PRINT_INFO);
	printInt(NUM_OF_BLOCKS, PRINT_INFO);
	printString("\n", PRINT_INFO);

	blockDesc = (blk_t*)HEAP_START_ADDR;
	blockDesc[0] = NUM_OF_BLOCKS;
	for (blk_t i = 1; i < NUM_OF_BLOCKS - 1; i++)
		blockDesc[i] = 0;
	blockDesc[NUM_OF_BLOCKS - 1] = NUM_OF_BLOCKS;
}

blk_t abs(blk_t desc) {
	return (desc >= 0)? desc : -desc;
}

blk_t next(blk_t index) {
	return index + abs(blockDesc[index]);
}

blk_t prev(blk_t index) {
	return index - abs(blockDesc[index - 1]);
}

blk_t end(blk_t index) {
	return index + abs(blockDesc[index]) - 1;
}

void zaglavlje() {
	if (!PRINT_MEMORY) return;
	printString("Blok segmenti:\t");
	printInt(blockDesc[0]);
	for (blk_t i = next(0); i < NUM_OF_BLOCKS; i = next(i)) {
		if (blockDesc[i] != 0) {
			printString(", ");
			printInt(blockDesc[i]);
		}
	}
	printString(".\n");
}

void *blkAlloc(size_t blocks) {
	printString("ALOKACIJA\n", PRINT_MEMORY);
	if ((size_t)free_blocks_count < blocks)
		return nullptr;
	blk_t count = (blk_t)blocks;
	for (blk_t i = 0; i < NUM_OF_BLOCKS; i = next(i)) {
		if (count <= blockDesc[i]) {
			// skrati segment za count
			blockDesc[i] = blockDesc[i] - count;
			blockDesc[end(i)] = blockDesc[i];
			i = next(i);
			// napravi od ostatka novi
			blockDesc[i] = -count;
			blockDesc[end(i)] = blockDesc[i];
			free_blocks_count -= count;
			zaglavlje();
			return (void*)(BLOCKS_START + i * MEM_BLOCK_SIZE);
		}
	}
	return nullptr;
}

void tryMergeNext(blk_t index) {
	blk_t nextIndex = next(index);
	if (nextIndex < NUM_OF_BLOCKS && blockDesc[nextIndex] > 0) {
		blockDesc[index] += blockDesc[nextIndex];
		blockDesc[nextIndex - 1] = 0;
		blockDesc[nextIndex] = 0;
		blockDesc[end(index)] = blockDesc[index];
	}
}

int blkFree(void *ptr) {
	printString("FREE\n", PRINT_MEMORY);
	if ((uint8*)ptr < BLOCKS_START || (uint8*)ptr >= BLOCKS_END || (size_t)ptr % MEM_BLOCK_SIZE != 0)
		return -1;
	blk_t index = (blk_t)((uint8*)ptr - BLOCKS_START) / MEM_BLOCK_SIZE;
	if (blockDesc[index] >= 0 || blockDesc[index] != blockDesc[end(index)] || blockDesc[index - 1] == 0)
		return -2;
	blockDesc[index] *= -1;
	blockDesc[end(index)] = blockDesc[index];
	// merge if next segemnt is also free
	tryMergeNext(index);
	// merge if prev segemnt is also free
	if (index > 0 && blockDesc[index - 1] > 0)
		tryMergeNext(prev(index));
	zaglavlje();
	return 0;
}

void *memAlloc(size_t bytes) {
	size_t blocks = (bytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
	return blkAlloc(blocks);
}

int memFree(void *ptr) {
	return blkFree(ptr);
}

}
