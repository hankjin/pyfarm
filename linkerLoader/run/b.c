#include <stdio.h>

void hack(){
    printf("Hack original%p\n", hack);
}
void* cur(){
    void * p;
    asm("movl 4(%%ebp), %0":"=r"(p));//next instuction

    return p;
}
__attribute__((weak)) extern void f();
int main(){
    int i = 4;
    void *p = cur()+0x17;
    asm("push %0 \n\t"//next
	    "push %1\n\t"//hack
	    "jmp %2\n\t"//f
	    ::"r"(p),"r"(hack),"r"(f));
    //call f, call hack, continue
    i = 5;
    return 0;
}

