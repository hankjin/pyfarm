#include "minicrt.h"
//*
#include "iostream"
using namespace hjz;
//*/
/*
#include <iostream>
using namespace std;
//*/

class D{
    public:
	D(){//no ~D, then no exit
	    printf("D, entered\n");
	}
	/*
	void f(){
	    cout<<"F of D:\n"<<endl;
	}{
	*/
};
class C{
    public:
	char *p;
	C(char *str){
	    printf("Hello, World:%s\n", str);
	    p = str;
//	    cout<<"Hello from cout"<<endl;
	}
	~C(){
	    printf("Bye %s\n", p);
	}
};
C c("hank");
C c2("hack");
D d;
void f(){
    printf("FFFF\n");
}
typedef void (*ctor_func)(void);
extern  ctor_func* ctors_begin;
extern ctor_func* crt_end;
int main(){
//    printf("Begin Ctor:%d, End:%d\n", ctors_begin,crt_end);
    C* dc = new C("Newed C");
    atexit_func_t vf = f;
    atexit(vf);
    C c2("hjz");
//    delete dc;
    return 0;
}
