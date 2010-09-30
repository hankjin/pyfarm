/*
ID: hankjin1
LANG: C++
TASK: ditch
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

//note: TLE of test 7
int N,M;
int ditch[201][201];
bool flag[201];
int seq[201];
int bestseq[201];
int minV;
int dfs(int point, int x){
    int res = 0, resI, temp;
    for(int i=2;i<=M;i++){
	if(ditch[point][i] != 0 && !flag[i]){
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
	for(int i=2;i<=M;i++){//find max path
	    if(ditch[1][i]!=0){
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
    int a,b;
    for(int i=0;i<N;i++){
	fin>>a>>b;
	fin>>ditch[a][b];
    }
    fout<<solve()<<endl;
    return 0;
}

