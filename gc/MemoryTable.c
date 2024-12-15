/*
typedef struct MemoryBlock {
	size_t size;
	// To the actual memory block this represents
	// Also the key in the hashtable; useful beause of linear probing
	void* ptr; 

	// all just stuff that may be convenient later
	struct MemoryBlock* next;
	struct MemoryBlock* prev;
	int free;
	int marked; 
};

typedef struct MemoryTable {
	size_t size; // of the underlying array I believe
	MemoryBlock* start; // start of arr	
};


MemoryBlock* 
*/

// Using struct since it'll be a struct when I actually use it later.
typedef struct Number {
	uintptr_t num;
	// linear probing, so we need to know what they key is
	// so we can match with it when linearly searching for value.
	int key;
};

Number createNumber(uintptr_t num){
	Number newNum;
	newNum.num = num;
	// TODO: set key with hasing function
	return newNum;
}

typedef struct HashTable {
	size_t size; // of underlying array, int bytes not elements
	Number* head;	
};



int main(){

}
