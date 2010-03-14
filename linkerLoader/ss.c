int printf(const char* format, ...);
int global_init_var = 84;
int global_uninit_var;//not appeared
//__attribute__((section(".hjz"))) int global_hjz;
void func1(int i){
    printf("%d\n",i);//rodata
}
int main(void){
    static int static_var = 85;//data
    static int static_var2;//bss
    static int static_var3 = 0;//bss
    int a = 1;
    int b;
    func1(static_var + static_var2+a+b);
    return a;
}
