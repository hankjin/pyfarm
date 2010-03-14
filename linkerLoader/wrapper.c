#include <stdio.h>
char *usage = "Usage: %s <elfname> [<output>]\n";
char *fname;
char * buffer, *encrypted;
int origSize, newSize;
void readElf();
void encrypt();
void makeElf();
int main(int argc, char* argv[]){
    if(argc==1)
	printf(usage, argv[0]);
    fname = argv[1];
    readElf();
    encrypt();
    makeElf();
    return 0;
}
//read elf from file fname into buffer, give value to origSize
void readElf(){
}
//encrypt bytes in buffer, give newSize to newSize
void encrypt(){
}
//create the elf file with encrypted 
void makeElf(){
    //extract the wrapper
    //dump the encrypt into wrapper
    //write wrapper into file
}
