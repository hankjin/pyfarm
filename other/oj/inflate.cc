/*
ID: hankjin1
LANG: C++
TASK: inflate
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

//note: notice the limit of N and M
int M,N;
struct Prob{
    int m;
    int p;
}prob[10000];
int L[10001];



int main(){
    ifstream fin("inflate.in");
    ofstream fout("inflate.out");
    fin>>M>>N;
    int p,m;
    for(int i=0;i<N;i++){
	fin>>p>>m;
	prob[i].p=p;
	prob[i].m=m;
    }
    for(int i=0;i<=M;i++){
	L[i]=0;
    }
    for(int i=0;i<N;i++){
	for(int j=prob[i].m;j<=M;j++){
	    if(L[j-prob[i].m]+prob[i].p > L[j]){
		L[j] = L[j-prob[i].m]+prob[i].p;
	    }
	}
    }
    fout<<L[M]<<endl;
 
    return 0;
}
