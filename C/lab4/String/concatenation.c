#include <stdio.h>
#include <string.h>

int main() {
    char s1[30];
    char s2[14] = "University of";
    char s3[15] = " C Programming";
    strcpy(s1, s2);
    /*
        strncat 删除了第一个string component末尾的null-terminate
        strncat 后面的数字n是允许被添加到s1后面的字符串长度，需要留一个
        位置给 null-terminate，所以要减去一

        减去一以后不需要声明： s1[29] = '\0';
        因为string里面空的部分本身就是由null-terminate填充的 
    */
    strncat(s1, s3, sizeof(s1) - strlen(s1) - 1);
    printf("%s\n", s1);
    return 0;
}
