#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

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

#define META_SIZE sizeof(blockMeta)

void *globalBase = NULL; // the head of our linked list

void main(){
	printf("hello malloc\n");
}

void *malloc(size_t size){
	blockMeta *block;
	
	if(globalBase == NULL){
		// Allocate our first memory from the OS.
		// Return that. Start of the linked list.
		block = requestSpace(NULL, size);	
		globalBase = block;
	} else {
		// Search our linked list for space big enough.
		// If it doesn't exist, then request for more
		// space from the OS.
		blockMeta *last = globalBase;	
		block = find_free_block(&last, size);
		if(!block){
			block = requestSpace(last, size);
			if(!block){
				return NULL;
			}
		} else {
			block->free = 0;
			// block->random shit here
		}
	}
	return block+1;	
}

void free(void* ptr){
	if(!ptr){
		return;
	}

	blockMeta* block = get_memblock(ptr);
	block->free = 1;
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



