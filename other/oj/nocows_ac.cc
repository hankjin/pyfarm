/*
ID: hankjin1
LANG: C++
TASK: nocows
*/
#include <iostream>
#include <fstream>
using namespace std;

int N,K;
int table[100][200];
int smalltree[100][200];
ifstream fin("nocows.in");
ofstream fout("nocows.out");

int main(){
    fin>>N>>K;
    int c;
    int kmax=2;
    table[1][1]=1;
    for(int i=2;i<=K;i++){
	kmax = N;//kmax*2<N ? kmax*2 : N;
	for(int j=1;j<=kmax;j+=2){
	    for(int k=1;k<=j-1-k;k+=2){
		if(k==j-1-k)
		    c = 1;
		else
		    c = 2;
		table[i][j] += c*(
			table[i-1][k]*smalltree[i-2][j-1-k]+
			table[i-1][j-1-k]*smalltree[i-2][k]+
			table[i-1][k]*table[i-1][j-1-k]);
		table[i][j]%=9901;
	    }
	}
	for(int j=0;j<=kmax;j++){
	    smalltree[i-1][j]+=table[i-1][j]+smalltree[i-2][j];
	    smalltree[i-1][j]%=9901;
	}
    }
    fout<<table[K][N]<<endl;
    return 0;
}


