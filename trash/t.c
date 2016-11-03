#include<stdio.h>
#include<string.h>

int main(){
    char name[256];
    while(1){
	printf("please enter your name \n");
	scanf("%255[^\n]\n",name);
	name[strcspn(name,"\r\n")] = '\0';
	if(strcmp(name,"q") == 0){
		break;
	}
	printf("This is my name : %s\n",name);
    }	
    return 0;
}
