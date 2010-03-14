#include "minicrt.h"

#define NULL 0
typedef struct _heap_header{
    enum{
	HEAP_BLOCK_FREE = 0xABABABAB,
	HEAP_BLOCK_USED = 0xCDCDCDCD,
    }type;
    unsigned size;
    struct _heap_header *next;
    struct _heap_header *priv;
} heap_header;
#define ADDR_ADD(a,o) (((char*)(a))+o)
#define HEADER_SIZE (sizeof(heap_header))

static heap_header * list_head = 0;
void free(void * ptr){
    heap_header* header = (heap_header*)ADDR_ADD(ptr,-HEADER_SIZE);
    if(header->type != HEAP_BLOCK_USED)
	return ;//shoud be fatal here
    header->type = HEAP_BLOCK_FREE;
    if(header->priv != NULL && header->priv->type == HEAP_BLOCK_FREE){
	header->priv->next = header->next;
	if(header->next != NULL)
	    header->next->priv = header->priv;
	header->priv->size += header->size;
	header = header->priv;
    }
    if(header->next != NULL && header->next->type==HEAP_BLOCK_FREE){
	header->size+= header->next->size;
	header->next = header->next->next;
	if(header->next != NULL)//added
	    header->next->priv = header;
    }
}
void *malloc(unsigned size){
    heap_header *header;
    if(size == 0)
	return NULL;
    header = list_head;
    while(header!=NULL){
	if(header->type == HEAP_BLOCK_USED){
	    header = header->next;
	    continue;
	}
	if(header->size > size+HEADER_SIZE &&
		header->size <= size + HEADER_SIZE*2){
	    header->type = HEAP_BLOCK_USED;
	    return ADDR_ADD(header, HEADER_SIZE);
	}
	if(header->size > size+HEADER_SIZE*2){
	    heap_header* next = (heap_header*)(ADDR_ADD(header,
			size+HEADER_SIZE));
	    next->priv = header;//1
	    next->next = header->next;//2
	    if(header->next!=NULL)//3
		header->next->priv = next;
	    next->type = HEAP_BLOCK_FREE;
	    next->size = header->size-(size+HEADER_SIZE);
	    header->next = next;//4
	    header->size = size+HEADER_SIZE;
	    header->type = HEAP_BLOCK_USED;
	    return ADDR_ADD(header, HEADER_SIZE);
	}
	header = header->next;
    }
    return NULL;
}
#ifndef WIN32
static int brk(void* end_data_segment){
    int ret = 0;
    asm("movl $45,%%eax	\n\t"
	    "movl %1,%%ebx	\n\t"
	    "int $0x80		\n\t"
	    "movl %%eax,%0	\n\t"
	    :"=r"(ret):"m"(end_data_segment));
}
#endif

#ifdef WIN32
#include <windows.h>
#endif
int mini_crt_heap_init(){
    void *base = NULL;
    heap_header *header = NULL;
    unsigned heap_size = 1024*1024*32;
#ifdef WIN32
    base = VirtualAlloc(0,heap_size,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(base == NULL)
	return 0;
#else
    base = (void*)brk(0);
    void* end = ADDR_ADD(base, heap_size);
    end = (void*)brk(end);
    if(!end)
	return 0;
#endif
    header = (heap_header*)base;
    header->size = heap_size;
    header->type = HEAP_BLOCK_FREE;
    header->next = NULL;
    header->priv = NULL;
    list_head = header;
    return 1;
}


