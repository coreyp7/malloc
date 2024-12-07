#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "mymalloc.h"

struct BlockMeta {
	size_t size; // size of this block of memory
	struct BlockMeta *next; // next memory block for this program
	struct BlockMeta *prev;
	int free; // is this block free or taken already?
	int magic; // 'For debugging only'
};
typedef struct BlockMeta BlockMeta;
#define META_SIZE sizeof(BlockMeta)

BlockMeta *_find_free_block(BlockMeta **last, size_t size);
BlockMeta *_requestSpace(BlockMeta* last, size_t size);
void print_heap_visualization(char* title);
void *my_malloc(size_t size);
void my_free(void* ptr);
BlockMeta* _split_block(BlockMeta* block, size_t size);
void print_block_info(BlockMeta* block);

void *globalBase = NULL; // the head of our linked list

void *my_malloc(size_t size){
	BlockMeta *block;
	
	if(globalBase == NULL){
		block = _requestSpace(NULL, size);	
		globalBase = block;
	} else { // will go in any call past first one

		// Search our linked list for space big enough.
		// If it doesn't exist, then request for more
		// space from the OS.
		BlockMeta *last = globalBase;	
		block = _find_free_block(&last, size);
		if(!block){
			block = _requestSpace(last, size);
			if(!block){
				return NULL;
			}
		} else {
			// if this block is big enough to accomodate a new block, let's split it.
			// ( + sizeof(unsigned char) is to set the minimum size for a mem block
			// to be 1 byte.
			// TODO: this should prolly have a test written for it.
			if(block->size > size + sizeof(BlockMeta) + sizeof(unsigned char)){
				printf("my_malloc: User requested for %zu bytes, the block is currently %zu bytes.Splitting block (BlockMeta at %p) since its really big.\n", size, block->size, block);
				print_heap_visualization("Before mem block split");	
				BlockMeta* newBlock = _split_block(block, size);
			} 

			block->free = 0;
			// block->random shit here
		}
	}
	printf("Returning block of size %zu to caller requesting %zu.\n", block->size, size);

	return block+1;	
}

void my_free(void* ptr){
	if(!ptr){
		return;
	}

	BlockMeta* block = (BlockMeta*)ptr -1;
	printf("my_free called: freeing %p\n", block);
	block->free = 1;
	print_block_info(block);
 	
	
	// Combine nearby blocks if possible.
	if(block->prev && block->prev->free){
		printf("my_free: Prev is free: combine block %p with block %p\n", block, block->prev);
		print_heap_visualization("my_free: Before combining");
		block->prev->size += META_SIZE + block->size;

		if(block->next){
			block->prev->next = block->next;
		} else {
			block->prev->next = NULL;
		}
		block->next = NULL;
		block->size = 0;
		block->prev = NULL;
		print_heap_visualization("my_free: After combining");
	}

	// Look at next and combine.
	if(block->next && block->next->free){
		printf("my_free: Next is free: combine block %p with block %p\n", block, block->next);
		print_heap_visualization("my_free: Before combining");
		block->size += META_SIZE + block->next->size;

		// OK now it is, just test it first, 
		// so leaving comment here for prosperity.
		BlockMeta* blockNext = block->next;
		if(block->next->next){
			block->next = block->next->next;
		} else {
			block->next = NULL;
		}
		blockNext->next = NULL;
		blockNext->size = 0;
		blockNext->prev = NULL;
		print_heap_visualization("my_free: After combining");
	}
}



/////////////////////////////////////////////
// Helper functions  			   //
/////////////////////////////////////////////

// Hey, find me a free block of space.
// Return a pointer to the memory block metadata.
// Returns null if one doesn't exist yet.
BlockMeta *_find_free_block(BlockMeta **last, size_t size){
	BlockMeta *curr = globalBase;
	// continue searching until we find a free block
	// big enough for us.
	while (curr && !(curr->free && curr->size >= size)){
		*last = curr;
		curr = curr->next;
	}
	return curr;
}

// NOTE: first time you call this, set last to NULL.
BlockMeta *_requestSpace(BlockMeta* last, size_t size){
	BlockMeta *block;
	block = sbrk(0);
	void *request = sbrk(size + META_SIZE);
	assert((void*)block == request); // Not thread safe?
	if(request == (void*) -1){
		return NULL; // gross sbrk failing convention
	}
	
	if(last){ // first time being called will be null
		last->next = block;
		block->prev = last;
	}
	block->size = size;
	block->next = NULL;
	block->free = 0;
	block->magic = 0x12345678; // idk wtf this is about yet
	return block;	
}

/*
Given the block to split & the size the block should be after the split,
will return a pointer to the new block created from the split.

The BlockMeta* block provided will be altered appropriately.

TODO: add assertion that makes sure the block is big enough to split.
*/
BlockMeta* _split_block(BlockMeta* block, size_t size){
	BlockMeta* newBlock = (BlockMeta*)block + 1 + size;
	size_t sizeOfNewBlock = block->size - size - META_SIZE;

	// Initialize newBlock, which will exist after
	// block in memory.
	newBlock->free = 1;
	newBlock->size = sizeOfNewBlock;
	if(block->next != NULL){
		newBlock->next = block->next;
	} else {
		newBlock->next = NULL;
	}
	newBlock->prev = block;
	
	// Update block with new size & next ref,
	// now that newBlock has taken the rest of block's memory.
	block->next = newBlock;
	block->size = size;
	printf("Created new block %p (size of %zu) after old block (%p) which now has size of (%zu).\n", newBlock, newBlock->size, block, block->size);
	if(newBlock->next){
		printf("oldblock(%p) -> newblock(%p) -> (%p)\n", block, newBlock, newBlock->next);
	} else {
		printf("oldblock(%p) -> newblock(%p) -> (NULL)\n", block, newBlock);
	}

}

// -----------------
// printing stuff
// -----------------

void print_heap_visualization(char* title){
	printf("-----\"%s\" VISUALIZATION START-----\n", title);
	printf("(metadata size is %zu)\n", META_SIZE);
	printf("global base:\n");
	BlockMeta* curr = globalBase;
	print_block_info(curr);
	while(curr->next != NULL) {
		curr = curr->next;
		print_block_info(curr);
		printf("|\nv\n");
	}
	printf("NULL\n");	
	printf("------\"%s\" VISUALIZATION END-----\n", title);
}


void print_block_info(BlockMeta* block){
	printf("----------\n");
	printf("starts at %p\n", block);
	printf("size: %zu\nprev: %p\nnext: %p\nfree: %i\n", block->size, block->prev, block->next, block->free);
	printf("----------\n");
}

