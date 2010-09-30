/**
ID: hankjin1
LANG: C++
TASK: milk6
*/
#include <iostream>
#include <fstream>
using namespace std;

ifstream fin("milk6.in");
ofstream fout("milk6.out");

int adj[33][33];
int cost[33][33];
int weight[33][33];
int path[33][33];
int N,M;

bool used[33];
int seq[33];
int hist[33][33];

int dfs(int point, int layer){
    used[point]=true;
    for(int i=1;i<=adj[point][0];i++){
	int u = adj[point][i];
	if(!used[u] && cost[point][u]>0){
	    seq[layer+1]=u;
	    if(u==N)
		return cost[point][N];
	    else{
		int c = dfs(u,layer+1);
		if(c>0)
		    return min(cost[point][u], c);
	    }
	}
    }
    used[point]=false;
    return 0;
}
int main(){
    fin>>N>>M;
    int a,b,c;
    for(int i=0;i<M;i++){
	fin>>a>>b>>c;
	adj[a][ ++adj[a][0] ]=b;
	cost[a][b]=c;
	weight[a][b]=c;
	path[a][b]=i+1;
    }
    int total = 0;
    int histN=0;
    while(true){
	fill_n(used,N+1,false);
	seq[1]=1;
	int res = dfs(1,1);
	if(res == 0)
	    break;
	total += res;
	for(int i=1;seq[i]!=N;i++){
	    cost[seq[i]] [seq[i+1]]-=res;
	    cost[seq[i+1]] [seq[i]]+=res;
	    hist[histN][ ++hist[histN][0] ]=path[seq[i]][seq[i+1]];
	}
    }
    cout<<total<<endl;
    return 0;
}
