#include <stdio.h>

extern int hashing(const char * string);

int main() {
    char* string = "A01";
    
    printf("The lenght of the string is : %d", hashing(string));

    return 0;
}