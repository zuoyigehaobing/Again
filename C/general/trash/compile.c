#include<stdio.h>
int main(){
	int * p;
	int a = 9;
	p = &a;
	char * my_str = "nihao";
	p = my_str;
	return 0;
}
