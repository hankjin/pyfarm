#include <elf.h>
#include <stdio.h>
extern char _binary_hjz_jpg_start, _binary_hjz_jpg_end;
char *s = &_binary_hjz_jpg_start;
char *e = &_binary_hjz_jpg_end;
int main(){
    printf("Elf:0x%x, ElfS:0x%x\n", 
	    sizeof(Elf32_Ehdr),
	    sizeof(Elf32_Shdr));
    printf("IMage:%d\n%c\n%s\n",
	    e-s,
	    _binary_hjz_jpg_start,
	    s);

    return 0;
}
