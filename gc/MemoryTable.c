#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Using struct since it'll be a struct when I actually use it later.
typedef struct {
	// linear probing, so we need to know what they key is
	// so we can match with it when linearly searching for value.
	uintptr_t key;

	// ptr to actual memory block this represents
	void* ptr;
} MemoryBlock;


typedef struct {
	size_t size; // of underlying array, int bytes not elements
	MemoryBlock* head;	
} HashTable;

size_t hash(void *ptr)
{
    return (((uintptr_t)ptr) >> 3) % 557;
}

void* allocate(size_t size, HashTable* table){
	// Call malloc and get memory from os.
	MemoryBlock memBlock;
	memBlock.ptr = malloc(size);
	memBlock.key = (uintptr_t)memBlock.ptr;
	
	// Use the address of the ptr returned from malloc
	// to put it into our map with our hashing function.
	table->head[hash(memBlock.ptr)] = memBlock;	
	printf("Put %p into index %li\n", memBlock.ptr, hash(memBlock.ptr));
	
	// TODO: handle collisions
	return memBlock.ptr;
}

void* deallocate(void* ptr, HashTable* table){
	// Find the element and free
	MemoryBlock* block = &table->head[hash(ptr)];
	free(block->ptr);
	block->key = -1;
	free(block); // this might be wrong
}

int main(){
	HashTable table;
	table.head = malloc(557 * sizeof(MemoryBlock));	
	table.size = 557 * sizeof(MemoryBlock);

	int* newInt = allocate(sizeof(int), &table);
}
