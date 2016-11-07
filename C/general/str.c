#include <stdio.h>

int main(){
	char *sweet = malloc(20* sizeof(char));
	*sweet = "tiramisu";
	printf("%s",*sweet);
	return 0;
}
