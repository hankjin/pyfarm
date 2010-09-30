/*
ID: hankjin1
LANG: C++
TASK: betsy
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

//note: case 6 OT
ifstream fin("betsy.in");
ofstream fout("betsy.out");
int N;
struct dir{
    int x,y;
}dirs[4]={ {-1,0},{1,0},{0,-1},{0,1}};
int res;
int path[7][7];
void walk(int x, int y, int v){
    if(v==N*N){
	if(x==N-1 && y==0)
	    res++;
	return ;
    }
    path[x][y]=v;
    for(int i=0;i<4;i++){
	int nx=x+dirs[i].x;
	int ny=y+dirs[i].y;
	if(nx<0 || nx==N || ny<0 || ny==N)
	    continue;
	if(0==path[nx][ny])
	    walk(nx,ny,v+1);
    }
    path[x][y]=0;
}
int main(){
    fin>>N;
    res=0;
    walk(0,0,1);
    fout<<res<<endl;
    return 0;
}
