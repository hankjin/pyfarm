/*
ID: hankjin1
LANG: C++
TASK: inflate
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

int M,N;
struct Prob{
    int m;
    int p;
}prob[10001];
int probN;
int L[10001][10001];
void add(int m, int p, int n){
    if((2*n-1)*m>=M){
	int t = (M-m*(n-1))/m;
	struct Prob tmp={m*t, p*t};
	prob[++probN]=tmp;
    }
    else{
	struct Prob tmp={m*n, p*n};
	prob[++probN]=tmp;
	add(m, p, n*2);
    }
}


int main(){
    ifstream fin("inflate.in");
    ofstream fout("inflate.out");
    fin>>M>>N;
    int p,m;
    for(int i=0;i<N;i++){
	fin>>p>>m;
	add(m,p,1);
    }
    for(int i=0;i<=M;i++){
	L[i][0]=0;
    }
    for(int i=0;i<=probN;i++){
	L[0][i]=0;
    }
    for(int i=1;i<=M;i++){
	for(int j=1;j<=probN;j++){
	    L[i][j] = L[i][j-1];
	    if(i>=prob[j].m){
		L[i][j] = max(L[i][j], L[i-prob[j].m][j-1]+prob[j].p);
	    }
	}
    }
    fout<<L[M][probN]<<endl;
 
    return 0;
}
