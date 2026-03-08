#include <stdio.h>

extern int hashing(const char * string);
extern int modulowith7(const char * string);
extern int fibonacci(const char * string);
extern int addhash(const char * string);
extern int asciicompare(const char * string);
extern int numberchange(const char * string);
extern int xorfunction(const char * string);

extern int result0;
extern int result1;
extern int result2;
extern int result3;

void read(char *buffer, int max_length) {
 
    printf("Enter a string: \n");
    scanf("%s", buffer);
		printf("The given string is: %s\n", buffer);
}


int main() {
		char string[100];
    
    read(string, sizeof(string));
    char* string0 = "A9b3";
	
    printf("The length of the given string is : %d \n", hashing(string));
		printf("The ascii result is : %d \n", asciicompare(string));
		printf("The numbers changed give: %d \n", numberchange(string));
		printf("The total hash is : %d \n", addhash(string));
		printf("The modulo with 7 is : %d \n", modulowith7(string));
    printf("The fibonacci of the number is : %d \n", fibonacci(string));
	
		numberchange(string0);
		printf("Hash result of A9b3 is: %d\n", result0);
		modulowith7(string0);
		printf("Modulo result of %d is: %d\n", result0, result1);
		fibonacci(string0);
    printf("Fibonacci  of %d result: %d\n", result1, result2);
		xorfunction(string0);
		printf("The bitwise xor of A9b3 is: %d\n", result3);
	
    return 0;
}