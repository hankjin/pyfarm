#include <elf.h>
#include <stdio.h>

__attribute__((weak)) g;
int main(){
    g = 3.4f;
    /*
    printf("%d\n", g);
    g = 'a';
    printf("%c\n", g);
    */
    printf("%f\n", g);

    return 0;
}
