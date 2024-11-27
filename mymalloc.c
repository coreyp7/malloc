#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "mymalloc.h"

struct blockMeta {
	size_t size; // size of this block of memory
	struct blockMeta *next; // next memory block for this program
	int free; // is this block free or taken already?
	int magic; // 'For debugging only'
};

typedef struct blockMeta blockMeta;

blockMeta *find_free_block(blockMeta **last, size_t size);
blockMeta *requestSpace(blockMeta* last, size_t size);
blockMeta *get_memblock(void* ptr);
void *my_malloc(size_t size);
void my_free(void* ptr);

#define META_SIZE sizeof(blockMeta)

void *globalBase = NULL; // the head of our linked list

/*
void main(){
	//printf("hello malloc\n");
	int* arr = my_malloc(10 * sizeof(int));
	//printf("address of allocated arr is %p\n", arr);
	my_free(arr);
	//printf("all done round 1\n");

	arr = my_malloc(5 * sizeof(int));
	//printf("this should use same memblock as before, because its smaller\n");
	//printf("start of block: %p\n", arr);
	my_free(arr);
}
*/

void *my_malloc(size_t size){
	blockMeta *block;
	
	// I always will go into this as long as I call this.
	if(globalBase == NULL){
		// Allocate our first memory from the OS.
		// Return that. Start of the linked list.
		block = requestSpace(NULL, size);	
		globalBase = block;
	} else { // will go in any call past first one

		// Search our linked list for space big enough.
		// If it doesn't exist, then request for more
		// space from the OS.
		blockMeta *last = globalBase;	
		block = find_free_block(&last, size);
		if(!block){
			printf("The requested size %lu isn't available. sbrk call.\n", size);
			// 1. will go in here when there is no existing block
			// big enough for the requested size in param.
			// 2. also when none of them are free
			// TODO: add function for splitting up bigger blocks
			// and updating the linkedlist.
			block = requestSpace(last, size);
			if(!block){
				// Only happens if OS can't offer any more memory.
				// am going to not worry about it.
				return NULL;
			}
		} else {
			printf("The requested size %lu is available. Will return the block at %p\n", size, block+1);
			// Will enter here if an existing block can accomodate
			// the request.
			block->free = 0;
			// block->random shit here
		}
	}
	//printf("I have allocated memory at %p\n", block+1);
	//printf("I have allocated memory at\n");
	return block+1;	
}

void my_free(void* ptr){
	if(!ptr){
		return;
	}

	blockMeta* block = get_memblock(ptr);
	block->free = 1;
	//printf("I have freed memblock at %p\n", ptr);
}


/////////////////////////////////////////////
// Helper functions  			   //
/////////////////////////////////////////////

// Hey, find me a free block of space.
// Return a pointer to the memory block metadata.
// Returns null if one doesn't exist yet.
blockMeta *find_free_block(blockMeta **last, size_t size){
	blockMeta *curr = globalBase;
	// continue searching until we find a free block
	// big enough for us.
	while (curr && !(curr->free && curr->size >= size)){
		*last = curr;
		curr = curr->next;
	}
	return curr;
}

// NOTE: first time you call this, set last to NULL.
blockMeta *requestSpace(blockMeta* last, size_t size){
	blockMeta *block;
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


// given a ptr of beginning of mem block; 
// get the memblock struct for this block of memory.
blockMeta *get_memblock(void* ptr){
	return (blockMeta*)ptr -1;
}



