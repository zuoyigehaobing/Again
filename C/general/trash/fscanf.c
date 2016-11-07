#include <stdio.h>
int main(){
	char * s = "123,4211,321,222";
	int a,b,c,d;
	fscanf(s,"%d,%d,%d,%d",&a,&b,&c,&d);
	printf("This is y a b c d: %d %d %d %d\n"a,b,c,d);
	return 0;
}
