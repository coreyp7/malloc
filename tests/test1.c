#include <stdio.h>
#include "../mymalloc.h"

void test_nothingBigEnough();
void test_noFreeBlocksThatAreBigEnough();
void test_splitBigBlock();
void test_combineBlocks();


/**
If you run these after each other, they'll share the
same pool system, so it won't behave exactly as you'd
expect when writing the test.

Run one at a time, but running back to back may allow
you to find situations you wouldn't otherwise.
*/
int main(){
	printf("main starting\n");
	test_nothingBigEnough();
	//test_noFreeBlocksThatAreBigEnough();
	//test_splitBigBlock();
	//test_combineBlocks();
	printf("main ending\n");
}

// this ones good
void test_combineBlocks(){
	printf("TEST START:%s\n", __func__);
	int* int1 = my_malloc(sizeof(int)); 
	int* int2 = my_malloc(sizeof(int)); 
	int* int3 = my_malloc(sizeof(int)); 
	int* int4 = my_malloc(sizeof(int)); 
	int* int5 = my_malloc(sizeof(int)); 
	my_free(int2);
	my_free(int4);
	// this free will trigger next & prev
	printf("HERE! this should be triggering next & prev\n");
	my_free(int3);
	
	
	printf("TEST END:%s\n", __func__);
	my_free(int1);
	my_free(int5);
}

// this ones good
void test_splitBigBlock(){
	printf("TEST START:%s\n", __func__);
	int* arr = (int *)my_malloc(150 * sizeof(int));
	for(int i=0; i<150; i++){
		arr[i] = i * 5;
	}
	my_free(arr);
	
	// Now there's a really big block,
	// it should be split up and given away.
	// All three of these will split up that big one.
	int* split = my_malloc(sizeof(int));
	int* split2 = my_malloc(sizeof(int));
	int* split3 = my_malloc(sizeof(int));
	int* splitArr = my_malloc(15 * sizeof(int));
	printf("TEST END:%s\n", __func__);
	my_free(split);
	my_free(split2);
	my_free(split3);
	my_free(splitArr);
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
