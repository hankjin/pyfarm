static int a = 4;
extern int b;
extern void ext();
void bar(){
    a = 1;
    b = 2;
}
void foo(){
    bar();
    ext();
}
