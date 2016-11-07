#include<stdio.h>
#include <string.h>

int main(int arcg,char **argv ){
	char *s1 = argv[1];
	char *s2 = argv[2];
	if(arcg != 3){
		printf("Invalid input");
		return 0;
	}
	int l1 = strlen(s1);
	int l2 = strlen(s2);
	int times = l1;
	if( l1 != l2){
		printf("Different");
		return 0;
		}
	for(int j = 0;j<times;j++){
		if(s1[j] != s2[j]){
			printf("Different");
			return 0;
		}
	}
	
	printf("Same");
	return 0;
}
