/*
ID: hankjin1
LANG: C++
TASK: ditch
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

//note: TLE of test 8 if no 0 dist path removed
//note: TLE of test 9
int N,M;
int ditch[201][201];
int path[201][201];

bool used[201];
int seq[201];
int bestseq[201];
int minV;
inline void add(int from, int to, int v){
    if(ditch[from][to]==0){
	path[from][ ++path[from][0] ] = to;
    }
    ditch[from][to]+=v;
}
inline void rm(int from, int to, int v){
    ditch[from][to]-=v;
    if(ditch[from][to]==0){
	for(int i=1;i<=path[from][0];i++){
	    if(path[from][i]==to){
		path[from][i]=path[from][ path[from][0] ];
		break;
	    }
	}
	path[from][0]--;
    }
}
int dfs(int point, int x){
    seq[x]=point;
    used[point]=true;
    for(int i=1; i<=path[point][0];i++){
	int j = path[point][i];
	if(!used[j]){
	    if(j == M){
		seq[x+1]=M;
		return ditch[point][j];
	    }
	    int res = dfs(j, x+1);
	    if(res > 0)
		return min(ditch[point][j], res);
	}
    }
    used[point]=false;
    return 0;
}
int solve(){
    int total = 0;
    while(true){
	fill_n(seq, M+1, 0);
	fill_n(used, M+1, false);
	int v = dfs(1, 1);//find max path
	if(v == 0)
	    break;
	total += v;
	for(int i=1; seq[i]!=M;i++){
	    rm(seq[i], seq[i+1], v);
//	    ditch[seq[i]][seq[i+1]] -= v;
	    add(seq[i+1], seq[i], v);
	}
    }
    return total;
}
int main(){
    ifstream fin("ditch.in");
    ofstream fout("ditch.out");
    fin>>N>>M;
    int a,b,c;
    for(int i=0;i<N;i++){
	fin>>a>>b>>c;
	add(a,b,c);
    }
    fout<<solve()<<endl;
    return 0;
}

