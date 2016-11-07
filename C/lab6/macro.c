#include <stdio.h>

// macros let you define keywords that are replaced by specified strings
// when your program is processed * before * being compiled. Which mean:
// the macro is evaluated during the pre-processing procedure

// the macro language isn't C, there is no equal sign while assigning
// also , no semicolomns
#define MAX_NAME_LENGTH 40
#define MY_MAX = 80
// if you decalre char nam[MY_MAX] , then it's actually char nam[=80],
// which will result in a compilation error.


// if you define an expression :
//	(1) no space between WITH_TAX and (x)
//	(2) put the parameter (x) into paranthesis to make sure x is fully 
//	    evaluated before you do the mult
//	(3) put the entire definition into paranthesis
//      
//   * see main function for example
#define WITH_TAX(x) ((x) * 1.13)
#define with_tax(x) (x * 1.13)

char name[MAX_NAME_LENGTH];  // <-> char name[40];before compilation


int main(){
	double p1 = 9.99;
	double p2 = 12.49;
	// 25.4
	printf("Capital With : (9.99+12.49)*1.13=%f\n",WITH_TAX(p1+p2));
	// 24.1
	printf("Small with : (9.99 + 12.49*1.13)=%f\n",with_tax(p1+p2));
	return 0;
}
