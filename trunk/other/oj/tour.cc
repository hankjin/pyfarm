/*
ID: hankjin1
LANG: C++
TASK: tour
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
using namespace std;

ifstream fin("tour.in");
ofstream fout("tour.out");
int N,V;
map<string,int> dict;
bool path[100][100];
int cache[100][100];

int solve(int a,int b){
    if(a>b) swap(a,b);
    if(cache[a][b]!=0) return cache[a][b];
    if(a+b==2*N-2) return 0;
    if(a==b && a+b>0) return -100;
    int res = -1;
    for(int i=a+1;i<N;i++){
	if(path[a][i])
	    res = max(res, 1+solve(i,b));
    }
    cache[a][b]=res;
    return res;
}
int main(){
    fin>>N>>V;
    string str;
    for(int i=0;i<N;i++){
	fin>>str;
	dict[str]=i;
    }
    string from,to;
    for(int i=0;i<V;i++){
	fin>>from>>to;
	path[dict[from]][dict[to]]=path[dict[to]][dict[from]]=true;
    }
    int res = solve(0,0);
    fout<<(res==2?1:res)<<endl;

    return 0;
}
