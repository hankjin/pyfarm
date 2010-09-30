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

//note: unbeleiveable, it is ACed
ifstream fin("fence3.in");
ofstream fout("fence3.out");
int N;
int midx,midy;
double best;
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

int main(){
    fin>>N;
    int sx,sy,ex,ey;
    int minx=INT_MAX,miny=INT_MAX,maxx=0,maxy=0;
    for(int i=0;i<N;i++){
	fin>>sx>>sy>>ex>>ey;
	p[i].sx=min(sx,ex);
	p[i].ex=max(sx,ex);
	p[i].sy=min(sy,ey);
	p[i].ey=max(sy,ey);
	minx = min(minx, min(sx,ex));
	maxx = max(maxx, max(sx,ex));
	miny = min(miny, min(sy,ey));
	maxy = max(maxy, max(sy,ey));
    }
    best = INT_MAX;
    //floodfill();
    //* bruce method can go through 6 cases
    //first locate the MAYBE pos
    for(int i=minx*10;i<maxx*10;i+=10){
	for(int j=miny*10;j<maxy*10;j+=10){
	    double temp = dist(i,j);
	    if(best  > temp){
		best = temp;
		midx = i;
		midy = j;
	    }
	}
    }
    //then detail the maybe pos
    for(int i=midx-10;i<midx+10;i++){
	if(i<0)continue;
	if(i>=N*10) break;
	for(int j=midy-10;j<midy+10;j++){
	    if(j<0) continue;
	    if(j>=N*10) break;
	    double temp = dist(i,j);
	    if(best  > temp){
		best = temp;
		midx = i;
		midy = j;
	    }
	}
    }

    //*/
    double outx = (double)midx/10,
	   outy = (double)midy/10;
    fout<<setprecision(1)<<setiosflags(ios::fixed)
	<<outx<<" "<<outy<<" "<<best<<endl;

    return 0;
}
