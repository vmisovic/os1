#include "../h/memory.hpp"
#include "../h/print.hpp"

void *__mem_cpy(void* to, void *from, size_t len) {
	// when to < from works always
	for (size_t i = 0; i < len; i++)
		*((uint8*)to + i) = *((uint8*)from + i);
	return to;
}

struct mem_seg_node {
	union {
		mem_seg_node *next_segment; // free segments
		uint64 checksum;   // used segments
	};
	size_t segment_size;
};

#define MIN_BLOCK_SIZE 32

static mem_seg_node *head = 0;
static size_t free_space_left;

uint64 checksumf(mem_seg_node *seg) {
	return (uint64)seg * seg->segment_size;
}

void try_merge(mem_seg_node *a, mem_seg_node *b) {
	// tryes to merge free segments if they are neighbours, links them if they are not
	if (a == 0 || b == 0)
		return;
	if ((uint8*)a + a->segment_size != (uint8*)b)
		a->next_segment = b;
	else {
		a->segment_size += b->segment_size;
		a->next_segment = b->next_segment;
	}
}

void *__mem_alloc(size_t bytes) { // first fit
	size_t required = sizeof(mem_seg_node) + bytes;
        required += ((required % MIN_BLOCK_SIZE != 0) ? MIN_BLOCK_SIZE - required % MIN_BLOCK_SIZE : 0);
	mem_seg_node *allcoated = 0;
	if (head == 0) {
		printString("\nHEAD =\t");
		head = (mem_seg_node*)HEAP_START_ADDR;
		printHex((uint64)head);
		head->next_segment = 0;
		printString("\nfree space =\t");
		free_space_left = (size_t)((uint8*)HEAP_END_ADDR - (uint8*)HEAP_START_ADDR);
		printHex((uint64)free_space_left);
		head->segment_size = free_space_left;
		printString("\nHEAP start: \t");
		printHex((uint64)HEAP_START_ADDR);
		printString("\nHEAP end: \t");
		printHex((uint64)HEAP_END_ADDR);
		printString("\nsizeof mem_seg_node: \t");
		printInt(sizeof(mem_seg_node));
		printString("\nrequired: \t");
		printInt(required);
		__putc('\n');
	}
	if (required < free_space_left) {
		mem_seg_node *node = head;
		mem_seg_node *prev = 0;
		while (node != 0 && node->segment_size < required) {
			prev = node;
			node = node->next_segment;
		}
		if (node == 0) {
			printString("velika fragmentacija");
			// ozbiljna greska
		}
		if (node->segment_size - required > MIN_BLOCK_SIZE) {
			node->segment_size -= required;
			allcoated = (mem_seg_node*)((uint8*)node + node->segment_size);
			allcoated->segment_size = required;
		} else { // whole segment
			if (prev != 0)
				prev->next_segment = node->next_segment;
			else
				head = node->next_segment;
			allcoated = node;
		}
		//allcoated->next_segment = 0; // ???
		allcoated->checksum = checksumf(allcoated);
		free_space_left -= required;
		return (void*)((uint8*)allcoated + sizeof(mem_seg_node));
	}
	return 0;
}

int __mem_free(void *ptr) {
	mem_seg_node *to_free = (mem_seg_node*)((uint8*)ptr - sizeof(mem_seg_node));
	mem_seg_node *curr = head;
	mem_seg_node *prev = 0;
	if ( (size_t)((uint8*)HEAP_END_ADDR - (uint8*)to_free) % MIN_BLOCK_SIZE != 0 ||
	     (uint8*)to_free < (uint8*)HEAP_START_ADDR ||
	     (uint8*)to_free + to_free->segment_size > (uint8*)HEAP_END_ADDR ||
	     to_free->checksum != checksumf(to_free) )
		return -1;
	while (curr != 0 && curr < to_free) {
		prev = curr;
		curr = curr->next_segment;
	}
	if ( (curr != 0 && (uint8*)to_free + to_free->segment_size > (uint8*)curr) ||
	     (prev != 0 && (uint8*)prev + prev->segment_size > (uint8*)to_free) )
	    	return -1;
	if (curr != 0)
		try_merge(to_free, curr);
	else
		to_free->next_segment = 0;
	if (prev != 0)
		try_merge(prev, to_free);
	else
		head = to_free;
	return 0;
}
