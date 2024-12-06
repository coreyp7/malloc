#include <stdio.h>
#include "../mymalloc.h"

void test_nothingBigEnough();
void test_noFreeBlocksThatAreBigEnough();
void test_splitBigBlock();


// Test case:
/*

Called when there's no blocks big enough to accomodate the call
(that is, no free blocks big enough).


1. There's literally none available that accomodate, but they're all free.

2. There's a block that *could* work, but it is not free right now.

In both of these cases, ask the OS for more memory please.

*/

int main(){
	printf("main starting\n");
	test_nothingBigEnough();
	test_noFreeBlocksThatAreBigEnough();
	test_splitBigBlock();
	printf("main ending\n");
}

void test_combineBigBlocks(){
	printf("TEST START:%s\n", __func__);
	/*
		Corey: shit to do when you get back on this.
		1. Look into segfault.	
		2. Create test for block combining.
	*/
	printf("TEST END:%s\n", __func__);
}

void test_splitBigBlock(){
	printf("TEST START:%s\n", __func__);
	int* arr = (int *)my_malloc(50 * sizeof(int));
	for(int i=0; i<50; i++){
		arr[i] = i * 5;
	}
	my_free(arr);
	
	// Now there's a really big block,
	// it should be split up and given away.
	int* split = my_malloc(sizeof(int));
	my_free(split);
	printf("TEST END:%s\n", __func__);
}

void test_nothingBigEnough(){
	printf("TEST START:%s\n", __func__);

	char* examplechar = my_malloc(sizeof(char));
	my_free(examplechar);
	int* exampleint = my_malloc(sizeof(int));
	my_free(exampleint);
	unsigned long* examplelong = my_malloc(sizeof(unsigned long));
	my_free(examplelong);
	char* examplestring = my_malloc(25 * sizeof(char));
	my_free(examplestring);

	printf("TEST END:%s\n", __func__);
}


void test_noFreeBlocksThatAreBigEnough(){
	printf("TEST START:%s\n", __func__);

	char* examplechar = my_malloc(sizeof(char));
	printf("this is the free call that breaks shit\n");
	my_free(examplechar);
	int* exampleint = my_malloc(sizeof(int));
	int* intForTest = my_malloc(sizeof(int));
	my_free(intForTest);
	my_free(exampleint);

	printf("TEST END:%s\n", __func__);
}



