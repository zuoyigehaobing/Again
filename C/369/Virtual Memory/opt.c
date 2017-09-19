#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#define MAXLINE 200

extern int memsize;

extern int debug;

extern struct frame *coremap;

extern char *tracefile;

addr_t *vaddr_list;
addr_t *addr_inmem;
int *distances;
int cur;
int lines;

int future_ref_distance(addr_t addr);

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	// Calculate distance for each vaddr in memory
	int i;
	int max_val;
	int max_frame;

	// Find the victim.
	max_val = distances[0];
	max_frame = 0;
	for(i = 0; i < memsize; i++) {
		// distances[i] cannot be -1 because opt_evict is only called
		// when all the frames are in use.
		if(distances[i] == 0) {
			// This means that this frame will never be referenced again.
			max_frame = i;
			break;
		} else if (distances[i] > max_val) {
			max_val = distances[i];
			max_frame = i;
		}
	}

	// Remove the victim, i.e., mark the frame NOT IN USE.
	distances[max_frame] = -1;
	return max_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	// Get the vaddr
	addr_t vaddr = vaddr_list[cur];
	cur++;

	// Update addr_inmem, which is used to record what vaddr is in memory
	int frame = p->frame >> PAGE_SHIFT;
	addr_inmem[frame] = vaddr;

	// Update the distances because one more vaddr is reference, 
	// so the distances are decremented.
	distances[frame] = future_ref_distance(addr_inmem[frame]) + 1;
	int i;
	for (i = 0; i < memsize; i++)
		if (distances[i] > 0)
			distances[i]--;
			
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	char buf[MAXLINE];
	FILE *fp;
	int i = 0;
	char type;
	
	/* Count the number of lines in the file, 
	 * in order to know how much memory needs to be allocated 
	 * for the array, which stores all the memory addresses from the file.
	 */
	fp = fopen(tracefile, "r");	
	if (!fp) {
		perror("file opened unsuccessfully\n");
		exit(1);
	}

	lines = 0;
	while(fgets(buf, MAXLINE, fp) != NULL)
		lines++;
	
	if (fclose(fp)) {
		perror("file closed unsuccessfully\n");
		exit(1);
	}
	
	/* Read memory addresses from the file into the array,
	 * which helps the opt algorithm decide which frame is 
	 * least likely to be used in the future.
	 */
	vaddr_list = (addr_t *)calloc(lines, sizeof(addr_t));
	fp = fopen(tracefile, "r");
	if (!fp) {
		perror("file opened unsuccessfully\n");
		exit(1);
	}

	while(fgets(buf, MAXLINE, fp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &(vaddr_list[i]));
			i++;
		}
	}
	
	if (fclose(fp)) {
		perror("file closed unsuccessfully\n");
		exit(1);
	}
	
	// Initialize some other data structures for opt.
	cur = 0;
	addr_inmem = (addr_t *)calloc(memsize, sizeof(addr_t));
	distances = (int *)calloc(memsize, sizeof(int));
	for (i = 0; i < memsize; i++) {
		addr_inmem[i] = 0;
		distances[i] = -1;
	}
}

/* Helper function for calculating how far away the next
 * reference of addr is.
 */
int future_ref_distance(addr_t addr) {
	int i;
	int is_ref_later;
	is_ref_later = 0;
	for (i = cur + 1; i < lines; i++) {
		if (vaddr_list[i] == addr) {
			is_ref_later = 1;
			break;
		}
	}
	if(is_ref_later) {
		return i - cur;	
	} else {
		return 0;	
	}
}
