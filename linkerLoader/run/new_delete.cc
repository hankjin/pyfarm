#include "minicrt.h"
extern "C" void* malloc(unsigned int);
extern "C" void free(void*);

void * operator new(unsigned int size){//new A() => new(sizeof(A))...
    printf("New called %d\n",size);
    return malloc(size);
}
void operator delete(void* p){
    printf("Del called \n");
    free(p);
}
void *operator new[](unsigned int size){
    printf("New[] called %d\n",size);
    return malloc(size);
}
void operator delete[](void* p){
    printf("del[] called %d\n");
    free(p);
}
