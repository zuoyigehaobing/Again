#include<stdlib.h>
#include<stdio.h>

int main(){
	char buf[200] = "nihaowodedaljkdjaslkdjlkasjd";
	printf("%d\n",strlen(buf));
	bzero(buf,200);
	printf("%d",strlen(buf));
	return 0;
}
