#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int arm;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	int frame;
	while (1) {
		if (coremap[arm].pte->frame & PG_REF) {
			coremap[arm].pte->frame &= ~PG_REF;
		}
		else {
			frame = arm;
			arm = (arm + 1) % memsize;
			break;
		}
		arm = (arm + 1) % memsize;
	}
	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	//The reference bit is already set in pagetable.c.
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	arm = 0;
}
