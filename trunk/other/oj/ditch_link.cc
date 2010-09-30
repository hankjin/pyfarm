/*
ID: hankjin1
LANG: C++
TASK: ditch
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

int N,M;
int ditch[201][201];
int ditchI[201][201];
int ditchLen[201];
bool flag[201];
int seq[201];
int bestseq[201];
int minV;
int dfs(int point, int x){
    int res = 0, resI, temp;
    for(int ii=0;ii<ditchLen[point];ii++){
	int i = ditchI[point][ii];
	if(!flag[i]){
	    seq[x]=i;
	    flag[i]=true;
	    if(i==M){
		temp = ditch[point][i];
	    }
	    else{
		temp = min(ditch[point][i], dfs(i,x+1));
	    }
	    if(temp>res){
		res = temp;
		resI = i;
	    }
	    flag[i]=false;
	}
    }
    seq[x]=resI;
    return res;
}
int solve(){
    int total = 0;
    while(true){
	fill_n(seq, M+1, 0);
	fill_n(flag, M+1, false);
	seq[1]=1;
	int best = 0, path;
	for(int ii=0;ii<ditchLen[1];ii++){//find max path
	    int i = ditchI[1][ii];
	    seq[2]=i;
	    flag[i]=true;
	    if(i==M)
		path = max(best, ditch[1][i]);
	    else{
		path = min(ditch[1][i],dfs(i, 3));
	    }
	    if(path>best){
		best = path;
		for(int i=1;i<=M;i++)
		    bestseq[i]=seq[i];
	    }
	    flag[i]=false;
	}
	if(best == 0)
	    break;
	total += best;
	for(int i=1; bestseq[i]!=M;i++){
	    ditch[bestseq[i]][bestseq[i+1]] -= best;
	    ditch[bestseq[i+1]][bestseq[i]] += best;
	}
    }
    return total;
}
int main(){
    ifstream fin("ditch.in");
    ofstream fout("ditch.out");
    fin>>N>>M;
    int a,b,v;
    for(int i=0;i<N;i++){
	fin>>a>>b>>v;
	int pos = ditchLen[a];
	if(ditch[a][b]==0){
	    ditchI[a][pos]=b;
	    ditchLen[a]++;
	}
	ditch[a][b]+=v;
    }
    fout<<solve()<<endl;
    return 0;
}

