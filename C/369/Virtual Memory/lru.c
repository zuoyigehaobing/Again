#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int *q;
int last;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	int frame = q[last - 1];
	q[last - 1] = 0;
	last--;
	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	int i;
	int frame = p->frame >> PAGE_SHIFT;
	for(i = 0; i < last; i++)
		if(q[i] == frame)
			break;
	
	// i == last means that the for loop never breaks, 
	// that is, frame is not found in the array;
	// temp is the value we want to insert into the array, 
	// pos is the index where the end of the array will be after inserting.
	int temp;
	int pos;
	int j;
	if (i == last) {
		temp = frame;
		pos = last;
		last++;
	} else {
		temp = q[i];
		pos = i;
	}
	// Insert the most recently used frame to the front.
	for (j = pos; j > 0; j--)
		q[j] = q[j - 1];
	q[0] = temp;
	
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	q = (int*)calloc(memsize, sizeof(int));
	last = 0;
}
