#include "minicrt.h"
typedef void (*ctor_func)(void);

ctor_func ctors_begin[1] __attribute__((section(".ctors"))) = 
{
//    (ctor_func)-1
};

void run_hooks(){
    const ctor_func* list = ctors_begin;
//    printf("Hook begin%d\t end %d\n",ctors_begin, ctors_end);
    while((int)*++list != -1){
	printf("Hook process\n");
	(**list)();
    }
    printf("Hook over\n");
}
