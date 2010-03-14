#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>
#define WORD(a,b) ((a<<8)+b)
int main(int argc, char* argv[]){
    unsigned char * buffer;
    char head[52];
    char * strtab;
    int ret, secStart,secSize, secNum, strIndex, strStart,strSize,temp,i;
    int fd = open(argv[1],O_RDONLY);
    //assert(fd>0);
    ret = read(fd, head, sizeof(head));
    //assert(ret>0);
    secStart=WORD(head[0x21],head[0x20]);
    //assert(secStart==0x114);
    secSize = WORD(head[0x2f],head[0x2e]);
    //assert(secSize==0x28);
    secNum = WORD(head[0x31],head[0x30]);
    //assert(secNum==0x0b);
    strIndex = WORD(head[0x33],head[0x32]);
    //assert(strIndex==0x08);
    //read section table
    buffer = (char*)malloc(sizeof(char)*secSize*secNum);
    ret=lseek(fd, secStart, SEEK_SET);
    //assert(ret>0);
    ret=read(fd, buffer, secSize*secNum);
    //assert(ret>0);

    strStart=WORD(buffer[strIndex*secSize+0x11],
	    buffer[strIndex*secSize+0x10]);
    strSize=WORD(buffer[strIndex*secSize+0x15],
	    buffer[strIndex*secSize+0x14]);
    //assert(strStart==0xc1);
    //assert(strSize==0x51);
    strtab=(char*)malloc(sizeof(char)*strSize);
    lseek(fd, strStart, SEEK_SET);
    ret=read(fd, strtab, strSize);
    //assert(ret>0);

    for(i = 1; i < secNum; i ++){
	temp = WORD(buffer[i*secSize+1],buffer[i*secSize+0]);
	strStart = WORD(buffer[i*secSize+0x11],
		buffer[i*secSize+0x10]);
	strSize=WORD(buffer[i*secSize+0x15],
		buffer[i*secSize+0x14]);

	printf("%16s\t Off:0x%.2x\t\t Size:0x%.2x\n",
		&strtab[temp], strStart, strSize);
    }
    
    return 0;
}
