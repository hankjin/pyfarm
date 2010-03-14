#include "minicrt.h"
typedef void (init_func) (void);

#ifdef WIN32
#pragma section(".CRT$XCA",long, read)
#pragma section(".CRT$XCZ",long,read)
//..
#else
void run_hooks();
extern "C" void do_global_ctors(){
    printf("do_global_ctors\n");
    run_hooks();
}
#endif
