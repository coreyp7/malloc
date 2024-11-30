#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "mymalloc.h"

struct BlockMeta {
	size_t size; // size of this block of memory
	struct BlockMeta *next; // next memory block for this program
	int free; // is this block free or taken already?
	int magic; // 'For debugging only'
};

typedef struct BlockMeta BlockMeta;

BlockMeta *_find_free_block(BlockMeta **last, size_t size);
BlockMeta *_requestSpace(BlockMeta* last, size_t size);
void print_heap_visualization();
void *my_malloc(size_t size);
void my_free(void* ptr);

#define META_SIZE sizeof(BlockMeta)

void *globalBase = NULL; // the head of our linked list

void *my_malloc(size_t size){
	BlockMeta *block;
	
	// I always will go into this as long as I call this.
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
			//printf("The requested size %lu is available. Will return the block at %p\n", size, block+1);
			// if the block is larger than what's requested, then split up the block.
			// TODO: put this into a function.

			// if this block is big enough to accomodate a new block, let's split it.
			// ( + sizeof(unsigned char) is to set the minimum size for a mem block
			// to be 1 byte.
			if(block->size > size + sizeof(BlockMeta) + sizeof(unsigned char)){
				printf("Splitting block at %p since its really big (memory actually starts at %p).\n", block, block+1);
				BlockMeta* newBlock = (BlockMeta*)block + 1 + size;
				size_t sizeOfNewBlock = block->size - size - META_SIZE;
				newBlock->free = 1;
				newBlock->size = sizeOfNewBlock;
				if(block->next != NULL){
					newBlock->next = block->next;
				} else {
					newBlock->next = NULL;
				}
				block->next = newBlock;
				block->size = size;
				printf("Created new block %p (size of %zu) after old block (%p) which now has size of (%zu).\n", newBlock, newBlock->size, block, block->size);
				if(newBlock->next){
					printf("oldblock(%p) -> newblock(%p) -> (%p)\n", block, newBlock, newBlock->next);
				} else {
					printf("oldblock(%p) -> newblock(%p) -> (NULL)\n", block, newBlock);
				}
			} 

			block->free = 0;
			// block->random shit here
		}
	}
	printf("Memory block starting at %p has been returning to caller requesting %zu bytes.\n", block + 1, size);

	print_heap_visualization();
	return block+1;	
}

void my_free(void* ptr){
	if(!ptr){
		return;
	}

	BlockMeta* block = (BlockMeta*)ptr -1;
	block->free = 1;
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
	}
	block->size = size;
	block->next = NULL;
	block->free = 0;
	block->magic = 0x12345678; // idk wtf this is about yet
	return block;	
}



void print_heap_visualization(){
	printf("heap visual call\n");
}

