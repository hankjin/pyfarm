#include <stdio.h>
#include <stdlib.h>
unsigned maximum = 0;
unsigned calc(){
    int count = 1;
    while(1){
	void *buffer = malloc(count);
	if(buffer){
	    count*=2;
	    free(buffer);
	}
	else{
	    break;
	}
    }
    printf("MAMX:%u\n", count);
}
int main(int argc, char* argv[]){
    calc();

    unsigned blocksize[] = {1024*1024, 1024,1};
    int i , count;
    for(i = 0; i < 3; i ++){
	for(count = 1; ; count++){
	    void *block = malloc(maximum + blocksize[i] * count);
	    if(block){
		maximum = maximum + blocksize[i] * count;
		free(block);
	    }
	    else{
		break;
	    }
	}
	printf("WOWO\n");
    }
    printf("maxmim malloc size = %u bytes\n", maximum);
}
