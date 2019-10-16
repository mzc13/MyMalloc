#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "mymalloc.h"

// Comment out this define for a different random seed for every run
#define time(NULL) 0

// Test Case A
int mallocThenFree(int numberOfLoops) {
	if (numberOfLoops < 1) {
		return -1;
	}
	int i = 0;
	char* ptr;
	for (i = 0; i < numberOfLoops; i++) {
		ptr = malloc(1);
		free(ptr);
	}
	return 1;
}

// Test Case B
int freeWhileMallocing(int numberOfLoops) {
	if (numberOfLoops < 1) {
		return -1;
	}
	char* pointers[numberOfLoops];
	int i = 1;
	int j = 0;
	for (i = 1; i <= numberOfLoops; i++) {
		pointers[i-1] = malloc(1);

		// i starts at 1 to make the modulo calculations simpler
		if ((i % 50) == 0) {
			for (j = (i - 50); j < i; j++) {
				free(pointers[j]);
			}
		}
	}
	return 1;
}

// Test Case C
int mallocOrFree(int maxAllocations) {
	if (maxAllocations < 1) {
		return -1;
	}
	/* The pointers array needs to be able to hold the maximum number of
	   pointers that can be stored */
	char* pointers[maxAllocations];
	// pointersIndex keeps track of the latest allocation
	int pointersIndex = 0;
	int mallocCount = 0;
	int randInt = rand();
	while (mallocCount < maxAllocations) {
		if (randInt % 2) {
			pointers[pointersIndex] = malloc(1);
			pointersIndex++;
			mallocCount++;
		}
		else {
			if (pointersIndex > 0) {
				// If pointersIndex > 0, there is a pointer that can be freed
				free(pointers[pointersIndex-1]);
				pointersIndex--;
			}
		}
		randInt = rand();
	}
	// Freeing any remaining pointers
	while (pointersIndex > 0) {
		free(pointers[pointersIndex - 1]);
		pointersIndex--;
	}
	return 1;
}

// Test Case D
int randomSizeMallocOrFree(int maxAllocations) {
	if (maxAllocations < 1) {
		return -1;
	}
	/* The pointers array needs to be able to hold the maximum number of
	   pointers that can be stored */
	char* pointers[maxAllocations];
	// The pointerSize array keeps track of the size of each allocation
	int pointerSize[maxAllocations];
	// pointersIndex keeps track of the latest allocation
	int pointersIndex = 0;
	int mallocCount = 0;
	// totalSpaceUsed keeps track of how much of myBlock is being used
	int totalSpaceUsed = 0;
	int randInt = rand();
	while ((mallocCount < maxAllocations)) {
		if (randInt % 2) {
			randInt = (rand() % 64) + 1;
			// Another pointer can only be malloced if myBlock isn't full
			if ((totalSpaceUsed + randInt + 2) > 4093) {
				randInt = rand();
				continue;
			}
			pointers[pointersIndex] = malloc(randInt);
			// The size of metadata is 2 bytes and that must be taken into account
			pointerSize[pointersIndex] = randInt + 2;
			totalSpaceUsed += randInt + 2;
			pointersIndex++;
			mallocCount++;
		}
		else {
			if (pointersIndex > 0) {
				free(pointers[pointersIndex - 1]);
				totalSpaceUsed -= pointerSize[pointersIndex - 1];
				pointersIndex--;
			}
		}
		randInt = rand();
	}
	// Freeing any remaining pointers
	while (pointersIndex > 0) {
		free(pointers[pointersIndex - 1]);
		pointersIndex--;
	}
	return 1;
}

// Test Case E
int saturateMemory(int numberOfLoops) {
	if (numberOfLoops < 1) {
		return -1;
	}
	// The myBlock array can fit at most 1365 pointers of size 1 based on our metadata size
	char* pointers[1365];
	int i = 0;
	int j = 0;
	// Need a variable to keep track of how many pointers have been allocated
	int pointersIndex = 0;
	int randInt = rand();
	for (i = 0; i < numberOfLoops; i++) {
		// There are four random cases that could happen
		switch (randInt % 4) {
		// In case 0, two pointers that each fill up half of myBlock are allocated
		case 0:
			pointers[0] = malloc(2046);
			pointers[1] = malloc(2046);
			pointersIndex += 2;
			break;
		/* In case 1, the first half of myBlock is allocated to one pointer and the rest
		   of it is full of 1 byte pointers */
		case 1:
			pointers[pointersIndex] = malloc(2046);
			pointersIndex++;
			for (j = 0; j < 682; j++) {
				pointers[pointersIndex] = malloc(1);
				pointersIndex++;
			}
			break;
		/* In case 2, the second half of myBlock is allocated to one pointer and the first
		   half of it is full of 1 byte pointers */
		case 2:
			for (j = 0; j < 682; j++) {
				pointers[pointersIndex] = malloc(1);
				pointersIndex++;
			}
			pointers[pointersIndex] = malloc(2046);
			pointersIndex++;
			break;
		// In the last case, the entire array is allocated to 1 byte pointers
		default:
			for (j = 0; j < 1365; j++) {
				pointers[pointersIndex] = malloc(1);
				pointersIndex++;
			}
			break;
		}
		for (j = 0; j < pointersIndex; j++) {
			// All the pointers are freed
			free(pointers[j]);
		}
		// Index keeping track of pointers is reset and a new random integer is generated
		pointersIndex = 0;
		randInt = rand();
	}
	return 1;
}

// Test Case F
int checkAllocations(int numberOfLoops, char* file, int line) {
	if (numberOfLoops < 1) {
		return -1;
	}
	// The myBlock array can fit at most 1365 pointers of size 1 based on our metadata size
	char* pointers[1365];
	int i = 0;
	int j = 0;
	for (i = 0; i < numberOfLoops; i++) {
		for (j = 0; j < 1365; j++) {
			// Fill the myBlock array with 1 byte pointers and assign them an arbitrary value
			pointers[j] = malloc(1);
			*pointers[j] = 'a';
		}
		for (j = 0; j < 1365; j++) {
			if (*pointers[j] != 'a') {
				/* If any of the pointers are not equal to the arbitrary value, there is a
				   problem with either the malloc function or the previous call to free */
				printf("Error caused by line %d in %s:\n Test Failed\n", line, file);
				return -1;
			}
			// Free the pointer after it's been checked
			free(pointers[j]);
		}
	}
	return 1;
}

long getMean(long times[], int size) {
	long total = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		total += times[i];
	}
	return total / size;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	struct timeval start, end;
	int numberOfRuns = 100;
	long seconds = 0;
	long testA[numberOfRuns];
	long testB[numberOfRuns];
	long testC[numberOfRuns];
	long testD[numberOfRuns];
	long testE[numberOfRuns];
	long testF[numberOfRuns];
	int i = 0;
	for (i = 0; i < numberOfRuns; i++) {
		// Test Case A
		gettimeofday(&start, NULL);
		mallocThenFree(150);
		gettimeofday(&end, NULL);
		testA[i] = end.tv_usec - start.tv_usec;

		// Test Case B
		gettimeofday(&start, NULL);
		freeWhileMallocing(150);
		gettimeofday(&end, NULL);
		testB[i] = end.tv_usec - start.tv_usec;
		
		// Test Case C
		gettimeofday(&start, NULL);
		mallocOrFree(50);
		gettimeofday(&end, NULL);
		testC[i] = end.tv_usec - start.tv_usec;

		// Test Case D
		gettimeofday(&start, NULL);
		randomSizeMallocOrFree(50);
		gettimeofday(&end, NULL);
		testD[i] = end.tv_usec - start.tv_usec;

		// Test Case E
		gettimeofday(&start, NULL);
		saturateMemory(10);
		gettimeofday(&end, NULL);
		seconds = end.tv_sec - start.tv_sec;
		testE[i] = ((seconds * 1000000) + end.tv_usec) - start.tv_usec;

		// Test Case F
		gettimeofday(&start, NULL);
		checkAllocations(10, __FILE__, __LINE__);
		gettimeofday(&end, NULL);
		seconds = end.tv_sec - start.tv_sec;
		testF[i] = ((seconds * 1000000) + end.tv_usec) - start.tv_usec;
	}
	printf("Test Case A Average Runtime:\n %ld microseconds\n", getMean(testA, numberOfRuns));
	printf("Test Case B Average Runtime:\n %ld microseconds\n", getMean(testB, numberOfRuns));
	printf("Test Case C Average Runtime:\n %ld microseconds\n", getMean(testC, numberOfRuns));
	printf("Test Case D Average Runtime:\n %ld microseconds\n", getMean(testD, numberOfRuns));
	printf("Test Case E Average Runtime:\n %ld microseconds\n", getMean(testE, numberOfRuns));
	printf("Test Case F Average Runtime:\n %ld microseconds\n", getMean(testF, numberOfRuns));
	return 0;
}
