/*
ID: hankjin1
LANG: C++
TASK: butter
*/
#include <iostream>
#include <fstream>
#include <climits>
#include <queue>
using namespace std;
//note: 1. use spfa
//note: 2. use linjie zhen

int N,P,C;
ifstream fin("butter.in");
ofstream fout("butter.out");
int dist[801][801];
int path[801][801];//adj matrix
int cow[500];
int res;
void spfa(int p){
    bool inq[801];//dec
    int way[801];
    fill_n(inq, P+1, false);//init
    fill_n(way,P+1, INT_MAX);
    queue<int> q;

    way[p]=0;
    q.push(p);
    inq[p]=true;//add p
    while(!q.empty()){
	int x = q.front();//get from queue
	q.pop();
	inq[x]=false;

	for(int i=1; i<=path[x][0]; i++){
	    int v = path[x][i];
	    if(way[x]+dist[x][v]<way[v]){
		way[v]=way[x]+dist[x][v];
		if(!inq[v]){ //put into queue
		    q.push(v);
		    inq[v]=true;
		}
	    }
	}
    }

    int total = 0;
    for(int i=0;i<N;i++){
	total+=way[cow[i]];
    }
    if(total < res)
	res = total;
}

int main(){
    fin>>N>>P>>C;
    for(int i=0;i<N;i++)
	fin>>cow[i];
    int from,to,v;
    for(int i=1;i<=P;i++){
	for(int j=1;j<=P;j++){
	    if(i==j)
		dist[i][j]=0;
	    else
		dist[i][j]=INT_MAX/2;
	}
    }
    for(int i=0;i<C;i++){
	fin>>from>>to>>v;
	dist[from][to]=dist[to][from]=v;
	path[from][0]++;
	path[from][ path[from][0] ]=to;
	path[to][0]++;
	path[to][ path[to][0] ]=from;
    }
    res = INT_MAX;
    for(int k=1;k<=P;k++){
	spfa(k);
    }
    fout<<res<<endl;
    return 0;
}
