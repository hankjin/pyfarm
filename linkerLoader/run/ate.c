#include <stdio.h>
#include <stdlib.h>

void hjz(){
    printf("Byte\n");
}
int main(){
    atexit(hjz); //register a function to be called when exit
    return 0;
}

