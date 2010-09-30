/*
ID: hankjin1
LANG: C++
TASK: cryptcow
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
using namespace std;

char dest[]="Begin the Escape execution at the Break of Dawn";
int dec(char *raw, int depth){
    if(strcmp(raw, dest)==0)
	return depth;
    int n = strlen(raw);
    for(int i=0;i<n;i++){
	if(raw[i]=='C'){
	    for(int j=i+1;j<n;j++){
		if(raw[j]=='O'){
		    for(int k=j+1;k<n;k++){
			if(raw[k]=='W'){
			    char temp[76];
			    temp[0]=0;
			    strncat(temp,raw,i);
			    strncat(temp,raw+j+1,k-j-1);
			    strncat(temp,raw+i+1,j-i-1);
			    strncat(temp,raw+k+1,n-k-1);
			    int res = dec(temp, depth+1);
			    if(res!=-1)
				return res;
			}
		    }
		}
	    }
	}
    }
    return -1;
}

int main(){
    FILE *fin = fopen("cryptcow.in","r");
    ofstream fout("cryptcow.out");
    char *raw;
    size_t len;
    getline(&raw, &len, fin);
    raw[strlen(raw)-1]=0;

    int res = dec(raw,0);
    if(res == -1){
	fout<<"0 0"<<endl;
    }
    else{
	fout<<"1 "<<res<<endl;
    }
    return 0;
}
