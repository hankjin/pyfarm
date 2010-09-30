/**
ID: hankjin1
LANG: C++
TASK: fc
*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>
using namespace std;

ifstream fin("fc.in");
ofstream fout("fc.out");

int N;
double midx,midy;
struct point{
    double x;
    double y;
    int space;
    double ang;
}points[10000];
bool used[10000];//kick out one by one
bool hcmp(struct point p1, struct point p2){
    if(p1.space < p2.space)
	return true;
    else if(p1.space > p2.space)
	return false;
    else
	return p1.ang < p2.ang;
}
inline double dist(int a, int b){
    return sqrt((points[a].x-points[b].x)*(points[a].x-points[b].x)+
	    (points[a].y-points[b].y)*(points[a].y-points[b].y));
}
inline bool ok(int a,int b, int c){
    double x1=points[b].x-points[a].x,
	   y1=points[b].y-points[a].y,
	   x2=points[c].x-points[a].x,
	   y2=points[c].y-points[a].y,
	   x3=midx-points[a].x,
	   y3=midy-points[a].y;
    return (x1*y2-y1*x2)*(x1*y3-y1*x3)>0;
}
int main(){
    fin>>N;
    for(int i=0;i<N;i++){
	fin>>points[i].x>>points[i].y;
	midx+=points[i].x;
	midy+=points[i].y;
    }
    midx /= N;
    midy /= N;
    for(int i=0;i<N;i++){
	//booshit, use atan2 is so simple
	double dx = points[i].x-midx;
	double dy = points[i].y-midy;
	if(dx==0){
	    points[i].space = dy>0 ? 2 : 6;
	}
	else if(dy==0){
	    points[i].space = dx>0 ? 8 : 4;
	}
	else{
	    if(dy>0 && dx>0)
		points[i].space = 1;
	    else if(dy>0 && dx<0)
		points[i].space = 3;
	    else if(dy<0 && dx<0)
		points[i].space = 5;
	    else
		points[i].space = 7;
	    points[i].ang = dy/dx;
	}
    }
    sort(points, points+N, hcmp);
    //make it
    int a=0,b=1,c=2;
    bool flag = true;
    fill_n(used, N, true);
    while(flag){//if no point kicked out, game over
	flag = false;
	for(int i=0;i<N;i++){//the loop should over more quickly
	    if(ok(a,b,c)){
		a = b;
		b = c;
	    }
	    else{
		flag = true;
		used[b]=false;
		b = c;
	    }
	    do{
		c=(c+1)%N;
	    }while(!used[c]);
	}
    }
    //calc it
    double res = 0;
    int last = 0;
    while(!used[last])
       	last++;
    int cur = last, next = (cur+1)%N;
    do{
	while(!used[next]) next=(next+1)%N;
	res += dist(cur,next);
	cur = next;
	next = (cur+1)%N;
    }while(cur != last);
    fout<<setprecision(2)<<setiosflags(ios::fixed)<<res<<endl;
    return 0;
}
