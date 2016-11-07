#include <stdio.h>

// version one : struct then typedef

// def struce with semicolomn
struct student{
	char first_name[20];
	char last_name[20];
	int year;
	float gpa;
}; 
// semiconlomn here
typedef struct student Student;

//  version two : combine struct and typedef
typedef struct student_good{
	char first_name[20];
	char last_name[20];
	int year;
	float gpa;
} Student_good;

int main(){
	Student stu1;
	Student * p1;
	Student_good stu2;
	Student_good *p2;
	return 0;
}
