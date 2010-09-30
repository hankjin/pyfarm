/**
ID: hankjin1
LANG: C++
TASK: transform
*/
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;
#define N 10
char data[N][N];
char changed[N][N];
char temp[N][N];
int n,i,j;
bool cmp(char src[][N], char dest[][N]){
    for(i=0;i<n;i++){
	for(j=0;j<n;j++){
	    if(src[i][j]!=dest[i][j])
		return false;
	}
    }
    return true;
}
bool myclock(){
    char temp2[N][N];
    memcpy(temp2,temp,N*N);
    for(i=0;i<n;i++){
	for(j=0;j<n;j++){
	    temp[j][n-1-i] = temp2[i][j];
	}
    }
    return cmp(temp,changed);
}
bool reflect(){
    for(i=0;i<n;i++){
	for(j=0;j<n;j++){
	    temp[i][j] = data[i][n-1-j];
	}
    }
    return cmp(temp,changed);
}
bool combine(){
    if(myclock())
	return true;
    else if(myclock())
	return true;
    else if(myclock())
	return true;
    return false;
}

int main(){
    int i,j;
    ifstream fin("transform.in");
    ofstream fout("transform.out");
    fin>>n;
    for(i = 0; i < n;i ++){
	for(j=0; j < n;j ++){
	    fin>>data[i][j];
	    temp[i][j]=data[i][j];
	}
    }
    for(i = 0; i < n;i ++){
	for(j=0; j < n;j ++){
	    fin>>changed[i][j];
	}
    }
    if(myclock())
	fout<<1;
    else if(myclock()){
	fout<<2;
    }
    else if(myclock()){
	fout<<3;
    }
    else if(reflect()){
	fout<<4;
    }
    else if(combine()){
	fout<<5;
    }
    else if(cmp(data,changed)){
	fout<<6;
    }
    else
	fout<<7;
    fout<<endl;

    return 0;
}
