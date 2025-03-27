#include <stdio.h>

extern int hashing(const char * string);
extern int asciicompare(const char * string);

int main() {
    char* string = "A0345";
    
    printf("The lenght of the string is : %d", asciicompare(string));

    return 0;
}