/*
ID: hankjin1
LANG: C++
TASK: stall4
*/
#include <iostream>
#include <fstream>
using namespace std;

#define MAX 402
int N,M;
int adj[MAX][MAX];
int used[MAX];
int mat[MAX];
bool crosspath(int k){
    for(int i=1;i<=adj[k][0];i++){
	int j = adj[k][i];
	if(!used[j]){
	    used[j]=true;
	    if(mat[j]==0 || crosspath(mat[j])){
		mat[j] = k;
		return true;
	    }
	}
    }
    return false;
}
int main(){
    ifstream fin("stall4.in");
    ofstream fout("stall4.out");
    fin>>N>>M;
    int size, b;
    for(int i=1;i<=N;i++){
	fin>>size;
	for(int j=1;j<=size;j++){
	    fin>>b;
	    adj[i][ ++adj[i][0] ] = b+N;
	}
    }
    int res = 0;
    for(int i=1;i<=N;i++){
	fill_n(used, MAX, 0);
	if(crosspath(i))
	    res++;
    }
    fout<<res<<endl;;
    return 0;
}
