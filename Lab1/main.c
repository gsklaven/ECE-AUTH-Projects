#include <stdio.h>

extern int hashing(const char * string);
extern int modulowith7(const char * string);
extern int fibonacci(const char * string);
extern int addhash(const char * string);
extern int asciicompare(const char * string);
extern int numberchange(const char * string);

extern int result0;
extern int result1;
extern int result2;

int main() {
    char* string = "A9b3";
	
    printf("The length of the string is : %d \n", hashing(string));
		printf("The ascii result is : %d \n", asciicompare(string));
		printf("The numbers changed give: %d \n", numberchange(string));
		printf("The total hash is : %d \n", addhash(string));
		printf("The modulo with 7 is : %d \n", modulowith7(string));
    printf("The fibonacci of the number is : %d \n", fibonacci(string));
	
		numberchange(string);
		printf("Hash result is: %d\n", result0);
		modulowith7(string);
		printf("Modulo result is: %d\n", result1);
		fibonacci(string);
    printf("Fibonacci result: %d\n", result2);
	
    return 0;
}