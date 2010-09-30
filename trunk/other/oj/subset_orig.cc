/**
ID: hankjin1
LANG: C++
TASK: subset
*/
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

ifstream fin("subset.in");
ofstream fout("subset.out");

int N;
int found;
int methods[40][1600];

int solve(int x){
    int i,j;
    int maxi = 0;
    methods[0][0]=1;
    for(i=1;i<N;i++){
	maxi += i;
	for(j=0;j<=maxi;j++){
	    methods[i][j]=methods[i-1][j];
	    if(j>=i)
		methods[i][j]+=methods[i-1][j-i];
	}
    }
    return methods[N-1][x];
}
int main(){
    fin>>N;
    int total = (1+N)*N/2;
    if(total%2 != 0){
	fout<<0<<endl;
	return 0;
    }
    int subs = total/2-N;
    memset(methods,0,sizeof(int)*40*40*40);
    found = solve(subs);
    fout<<found<<endl;

    return 0;
}
