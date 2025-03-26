#include <stdio.h>

extern int hashing(const char * string);

int main() {
    char* string = "Hello 0 1";
    
    printf("The lenght of the string is : %d", hashing(string));

    return 0;
}