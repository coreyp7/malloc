#include <stdio.h>
#include "../mymalloc.h"

void test_nothingBigEnough();

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
	printf("main ending\n");
}



void test_nothingBigEnough(){
	printf("test start\n");

	char* exampleChar = my_malloc(sizeof(char));
	my_free(exampleChar);
	int* exampleInt = my_malloc(sizeof(int));
	my_free(exampleInt);
	unsigned long* exampleLong = my_malloc(sizeof(unsigned long));
	my_free(exampleLong);
	char* exampleString = my_malloc(25 * sizeof(char));
	my_free(exampleString);

	printf("test end\n");
}
