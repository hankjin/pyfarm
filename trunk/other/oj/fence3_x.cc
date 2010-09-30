/**
ID: hankjin1
LANG: C++
TASK: fence3
*/
#include <iostream>
#include <fstream>
#include <iomanip>
#include <climits>
#include <algorithm>
#include <cmath>
using namespace std;

//mountain climbing, unable to prove it works, but it works.
//but it does NOT work in USACO
ifstream fin("fence3.in");
ofstream fout("fence3.out");
int N;
int midx,midy;
double best,oldlen;
struct point{
    int sx,sy;
    int ex,ey;
}p[150];
bool used[1000][1000];

struct d{
    int x,y;
}dirs[8]={ {-1,-1},{-1,0},{-1,1}, {0,-1}, {0,1}, {1,-1},{1,0},{1,1}};

inline double dist(int ix,int iy){
    double x = (double)ix/10,
	   y = (double)iy/10;
    double res=0;
    for(int i=0;i<N;i++){
	if(p[i].sx<x && p[i].ex>x){
	    res+=abs(p[i].sy-y);
	}
	else if(p[i].sy<y && p[i].ey>y){
	    res+=abs(p[i].sx-x);
	}
	else{
	    double dx=min(abs(p[i].sx-x), abs(p[i].ex-x)),
		   dy=min(abs(p[i].sy-y), abs(p[i].ey-y));
	    res+=sqrt(dx*dx+dy*dy);
	}
    }
    return res;
}
void mountainclimb(){
    //current state
    oldlen = best;
    double temp;
    int ix=midx, iy=midy;

    int nx,ny;
    for(int i=0;i<8;i++){
	nx = ix+dirs[i].x;
	ny = iy+dirs[i].y;
	if(nx<0 || ny<0 || nx==N*10 || ny==N*10 || used[nx][ny])//out
	    continue;
	used[nx][ny]=true;
	temp = dist(nx,ny);
	if(best > temp){
	    best = temp;
	    midx = nx;
	    midy = ny;
	}
    }
    if(best < oldlen){//better solution available
	mountainclimb();
    }
}
int main(){
    fin>>N;
    double sumx=0, sumy=0;
    int sx,sy,ex,ey;
    for(int i=0;i<N;i++){
	fin>>sx>>sy>>ex>>ey;
	p[i].sx=min(sx,ex);
	p[i].ex=max(sx,ex);
	p[i].sy=min(sy,ey);
	p[i].ey=max(sy,ey);
	sumx+=p[i].sx+p[i].ex;
	sumy+=p[i].sy+p[i].ey;
    }
    best = INT_MAX;
    midx=(int)(sumx*10/(2*N));
    midy=(int)(sumy*10/(2*N));
    best = dist(midx,midy);
    used[midx][midy]=true;
    mountainclimb();

    //*/
    double outx = (double)midx/10,
	   outy = (double)midy/10;
    fout<<setprecision(1)<<setiosflags(ios::fixed)
	<<outx<<" "<<outy<<" "<<best<<endl;

    return 0;
}
