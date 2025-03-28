#include <stdio.h>

extern int hashing(const char * string);
extern int modulowith7(const char * string);
extern int fibonacci(const char * string);
extern int result;

int main() {
    char* string = "A0345a";
	
    printf("The length of the string is : %d \n", hashing(string));
		printf("The length of the string is : %d \n", modulowith7(string));
    printf("The length of the string is : %d \n", fibonacci(string));
	
		fibonacci(string);
    printf("Fibonacci result: %d\n", result);
	
    return 0;
}