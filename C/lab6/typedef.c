#include <stdio.h>

//typedef + type + name; You are not creating a new type, but provide
// a new name to the current existing type


typedef unsigned int age_t;
typedef unsigned int shoe_size_t;

void print_boots_size(shoe_size_t shoe_size){
	printf("This is my boot size : %d\n",shoe_size+2);
}

int main(){
	age_t my_age = 5;
	// the following things will compile without error or 
	// even warning since 
	// age_t is also an unsigned int 
	// string '5' can be interpreted into unsigned int
	print_boots_size(my_age); // -> 7
	print_boots_size('5'); // -> 55
	return 0;
}
