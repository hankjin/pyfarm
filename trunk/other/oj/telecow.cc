/*
ID: hankjin1
LANG: C++
TASK: telecow
*/
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>
using namespace std;

ifstream fin("telecow.in");
ofstream fout("telecow.out");
int N,M,c1,c2;
bool path[101][101];
int mark[101];
int seq[101];
int dfs(int from,int no){
    if(from==c2){
	return no;
    }
    mark[from]=no;
    seq[no]=from;
    for(int i=1;i<=N;i++){
	if(mark[i]==0 && path[from][i]){
	    int t=dfs(i,no+1);
	    if(t>0) return t;
	}
    }
    mark[from]=0;
    return 0;
}
int main(){
    fin>>N>>M>>c1>>c2;
    int a,b;
    for(int i=0;i<M;i++){
	fin>>a>>b;
	path[a][b]=path[b][a]=true;
    }
    int res=dfs(c1,1);
    fout<<(res-2)<<endl<<seq[2];
    for(int i=3;i<res;i++){
	fout<<" "<<seq[i];
    }
    fout<<endl;
    return 0;
}
