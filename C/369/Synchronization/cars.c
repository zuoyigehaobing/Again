#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "traffic.h"

extern struct intersection isection;

/**
 * Populate the car lists by parsing a file where each line has
 * the following structure:
 *
 * <id> <in_direction> <out_direction>
 *
 * Each car is added to the list that corresponds with
 * its in_direction
 *
 * Note: this also updates 'inc' on each of the lanes
 */
void parse_schedule(char *file_name) {
	int id;
	struct car *cur_car;
	struct lane *cur_lane;
	enum direction in_dir, out_dir;
	FILE *f = fopen(file_name, "r");

	/* parse file */
	while (fscanf(f, "%d %d %d", &id, (int*)&in_dir, (int*)&out_dir) == 3) {

		/* construct car */
		cur_car = malloc(sizeof(struct car));
		cur_car->id = id;
		cur_car->in_dir = in_dir;
		cur_car->out_dir = out_dir;

		/* append new car to head of corresponding list */
		cur_lane = &isection.lanes[in_dir];
		cur_car->next = cur_lane->in_cars;
		cur_lane->in_cars = cur_car;
		cur_lane->inc++;

	}

	fclose(f);
}

/**
 * TODO: Fill in this function
 *
 * Do all of the work required to prepare the intersection
 * before any cars start coming
 *
 */
void init_intersection() {
	int i;
	for(i=0;i<4;i++){

		/* initialize the lock of quad[i] , and check whether successful */
		if(pthread_mutex_init(&isection.quad[i],NULL) != 0){
			perror("mutex initialization\n");
		}

		/* initialize the lock of lanes[i] , and check whether successful */
		if(pthread_mutex_init(&isection.lanes[i].lock,NULL) != 0){
			perror("mutex initialization\n");
		}

		/* initialize the producer condition variable of lanes[i] , and check whether successful */
		if(pthread_cond_init(&isection.lanes[i].producer_cv,NULL) != 0){
			perror("condition variable initialization\n");
		}

		/* initialize the consumer condition variable of lanes[i] , and check whether successful */
		if(pthread_cond_init(&isection.lanes[i].consumer_cv,NULL) != 0){
			perror("condition variable initialization\n");
		}

		/* initialize the in_cars,out_cars,inc,passed,buffer,head,tail,capacity and in_bun of the current lane */
		isection.lanes[i].in_cars = NULL;
		isection.lanes[i].out_cars = NULL;
		isection.lanes[i].inc = 0;
		isection.lanes[i].passed = 0;
		isection.lanes[i].buffer = (struct car **)calloc(LANE_LENGTH,sizeof(struct car *));
		isection.lanes[i].head = 0;
		isection.lanes[i].tail = 0;
		isection.lanes[i].capacity = LANE_LENGTH;
		isection.lanes[i].in_buf = 0;
	}

}

/**
 * TODO: Fill in this function
 *
 * Populates the corresponding lane with cars as room becomes
 * available. Ensure to notify the cross thread as new cars are
 * added to the lane.
 *
 */
void *car_arrive(void *arg) {

	struct lane *l = arg;

	/* avoid compiler warning */
	l = l;


	while(l->in_cars){
		pthread_mutex_lock(&l->lock);
		while (l->in_buf == l->capacity){
			pthread_cond_wait(&l->consumer_cv,&l->lock);
		}

		l->buffer[l->tail] = l->in_cars;

		/* Update in_cars */
		l->in_cars = l->in_cars->next;
		l->inc --;

		/* Update buffer */
		l->buffer[l->tail]->next = NULL;
		l->tail = (l->tail + 1) % l->capacity;
		l->in_buf ++;

		pthread_cond_signal(&l->producer_cv);
		pthread_mutex_unlock(&l->lock);

	}

	return NULL;
}

/**
 * TODO: Fill in this function
 *
 * Moves cars from a single lane across the intersection. Cars
 * crossing the intersection must abide the rules of the road
 * and cross along the correct path. Ensure to notify the
 * arrival thread as room becomes available in the lane.
 *
 * Note: After crossing the intersection the car should be added
 * to the out_cars list of the lane that corresponds to the car's
 * out_dir. Do not free the cars!
 *
 *
 * Note: For testing purposes, each car which gets to cross the
 * intersection should print the following three numbers on a
 * new line, separated by spaces:
 *  - the car's 'in' direction, 'out' direction, and id.
 *
 * You may add other print statements, but in the end, please
 * make sure to clear any prints other than the one specified above,
 * before submitting your final code.
 */
void *car_cross(void *arg) {
	struct lane *l = arg;

	/* Avoid compiler warning */
	l = l;

	/* Continue arranging cars to pass across the intersection if there are some cars waiting*/
	while(l->in_buf != 0 || l->inc != 0){


		/* Wait until some car goes into the lane(buffer)  */
		pthread_mutex_lock(&l->lock);
		while (l->in_buf == 0 && l->inc != 0){
			pthread_cond_wait(&l->producer_cv,&l->lock);
		}


		/* Get the first car in the lane that is ready to pass
		 * compute the path based on its direction
		 * and compute the path-length of that path        
		 */
		struct car * cur = l->buffer[l->head];
		int *path = compute_path(cur->in_dir,cur->out_dir);
		int pathlength;
		for(pathlength=1;pathlength<3;pathlength++){
			if(path[pathlength] == -1){
				break;
			}
		}

		/* Lock the quad appear in the path from high priority to low priority*/
		int i = 0;
		while (i < pathlength){
			pthread_mutex_lock(&isection.quad[path[i]-1]);
			i++;
		}

		/* Update the changed variable when the car is passing the intersection,and make a printf to the standard out */
		l->in_buf--;
		l->head = (l->head + 1) % l->capacity;

		/* No need to lock tol, there will be no race condition, 
		 * because intersection guarantees that only one car at a time can be wriiten to out_cars  
		 */
		struct lane * tol = &isection.lanes[cur->out_dir];
		tol->passed += 1;
		cur->next = tol->out_cars;
		tol->out_cars = cur;
		fprintf(stdout,"in_dir : %d ; out_dir : %d; id %d\n",cur->in_dir,cur->out_dir,cur->id);

		/*  Unlock the quad lock from low priority to high priority */
		i = pathlength;
		while (i > 0){
    		pthread_mutex_unlock(&isection.quad[path[i-1]-1]);
			i--;
		}

		pthread_cond_signal(&l->consumer_cv);
		pthread_mutex_unlock(&l->lock);

		free(path);
	}
	return NULL;
}

/**
 * TODO: Fill in this function
 *
 * Given a car's in_dir and out_dir return a sorted
 * list of the quadrants the car will pass through.
 *
 */
int *compute_path(enum direction in_dir, enum direction out_dir) {
	/* The caller is responsible for freeing the array result */
	int * result = (int*)calloc(3,sizeof(int));

	/* Computes the path with given in_dir and out_dir, 
	 * -1 represents that the element of the array is not used
	 */
	switch(in_dir) {
		case NORTH:
			switch(out_dir) {
				case NORTH: result[0]=1;result[1]=2; result[2]=-1; break;
				case WEST:  result[0]=2;result[1]=-1;result[2]=-1; break;
				case SOUTH: result[0]=2;result[1]=3; result[2]=-1; break;
				case EAST:  result[0]=2;result[1]=3; result[2]=4;  break;
				default: result[0]=-1;result[1]=-1; result[2]=-1;
			} break;
		case WEST:
			switch(out_dir) {
				case NORTH: result[0]=1;result[1]=3; result[2]=4;  break;
				case WEST:  result[0]=2;result[1]=3; result[2]=-1; break;
				case SOUTH: result[0]=3;result[1]=-1;result[2]=-1; break;
				case EAST:  result[0]=3;result[1]=4; result[2]=-1; break;
				default: result[0]=-1;result[1]=-1; result[2]=-1;
			} break;
		case SOUTH:
			switch(out_dir) {
				case NORTH: result[0]=1;result[1]=4; result[2]=-1; break;
				case WEST:  result[0]=1;result[1]=2; result[2]=4;  break;
				case SOUTH: result[0]=3;result[1]=4; result[2]=-1; break;
				case EAST:  result[0]=4;result[1]=-1;result[2]=-1; break;
				default: result[0]=-1;result[1]=-1; result[2]=-1;
			} break;
		case EAST:
			switch(out_dir) {
				case NORTH: result[0]=1;result[1]=-1;result[2]=-1; break;
				case WEST:  result[0]=1;result[1]=2;result[2]=-1;  break;
				case SOUTH: result[0]=1;result[1]=2;result[2]=3;   break;
				case EAST:  result[0]=1;result[1]=4;result[2]=-1;  break;
				default: result[0]=-1;result[1]=-1; result[2]=-1;
			} break;
		default:
			result[0]=-1;result[1]=-1; result[2]=-1;
	}

	return result;
}
