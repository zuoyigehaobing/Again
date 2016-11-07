#include<stdio.h>
#include<string.h>

void reverse_name(char *src,char *dest){
	char * space;
	space = strchr(src,' ');
	int index = 0;
	strcpy(dest,(space + 1));
	index = strlen(dest);
	dest[index] = ' ';
	index ++;

	printf("Using strlen : %d\n",strlen(dest));
	printf("using index : %d\n",index);
	printf("Dest in the middle : %s\n",dest);	
	// you can't minus strlen(dest) since it's more than what you want
	int first_name_len = strlen(src) - strlen(dest);
	strncpy((dest+index),src,first_name_len);
	index += first_name_len;
	dest[index] = '\0';
}

int main(int argc,char *agrv[]){
	char s[50] = "shan bingzhao";
	char sr[50];
	reverse_name(s,sr);
	printf("This is my s: %s\n",s);
	printf("This is my sr: %s\n",sr);
	return 0;
}

