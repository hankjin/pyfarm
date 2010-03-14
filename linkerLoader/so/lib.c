#include <stdio.h>

extern int x;	//when extern used, -shared can not make them share
		//only -fPIC is ok
void foobar(int i){
    int k = 4;
    printf("foobar @:%p\n",foobar);
    printf("Printing from lib.so %d! x=%d\n", i, x);
    sleep(-1);
}
