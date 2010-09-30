/*
ID: hankjin1
LANG: C++
TASK: butter
*/
#include <iostream>
#include <fstream>
#include <climits>
using namespace std;
//note: floyd-mashall, 1-8 passed, but overtime at 9

int N,P,C;
ifstream fin("butter.in");
ofstream fout("butter.out");
int dist[801][801];
int cow[500];

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
    }
    for(int k=1;k<=P;k++){
	for(int i=1;i<=P;i++){
	    for(int j=1;j<=P;j++){
		if(dist[i][k]+dist[k][j]<dist[i][j])
		    dist[i][j]=dist[i][k]+dist[k][j];
	    }
	}
    }
    int min = INT_MAX;
    for(int i=1;i<=P;i++){
	int total = 0;
	for(int j=0;j<N;j++){
	    total+=dist[i][cow[j]];
	}
	if(total < min)
	    min = total;
    }
    fout<<min<<endl;
    return 0;
}
