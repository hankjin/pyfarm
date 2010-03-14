#include "minicrt.h"
#ifdef WIN32
#include <windows.h>
#endif

extern int main(int argc, char* argv[]);
void exit(int);
static void crt_fatal_error(const char* msg){
    printf(msg);
    exit(1);
}
void mini_crt_entry(void){
    int ret;
#ifdef WIN32
    int flag = 0;
    int argc = 0;
    char* argv[16];
    char* c1 = GetCommandLineA();
    argv[0] = c1;
    argc++;
    while(*c1){
	if(*c1 == '\"')
	    if(flag == 0)
		flag = 1;
	    else
		flag = 0;
	else if(*c1==' ' and flag==0){
	    if(*(c1+1)){
		argv[argc]=c1+1;
		argc++;
	    }
	    *c1 = '\0';
	}
	c1++;
    }
#else
    int argc;
    char ** argv;
    char* ebp_reg = 0;
    asm("movl %%ebp,%0 \n":"=r"(ebp_reg));
    argc = *(int*)(ebp_reg+4);
    argv = (char**)(ebp_reg+8);
#endif
    if(!mini_crt_heap_init())
	crt_fatal_error("heap init failed");
    if(!mini_crt_io_init())
	crt_fatal_error("IO init failed");
    do_global_ctors();
    ret = main(argc, argv);
    exit(ret);
}
void exit(int exitCode){
    mini_crt_call_exit_routine();
#ifdef WIN32
    ExitProcess(exitCode);
#else
    asm("movl %0,%%ebx \n\t"
	    "movl $1,%%eax\n\t"
	    "int $0x80 \n\t"
	    "hlt\n\t"::"m"(exitCode));
#endif
}
