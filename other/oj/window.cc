/*
ID: hankjin1
LANG: C++
TASK: window
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
using namespace std;

ifstream fin("window.in");
ofstream fout("window.out");

class win{
    public:
	int x,y,X,Y;
	int d;
	bool ready;
	win(){
	    ready=false;
	}
	win(int m,int n,int o,int p,int q){
	    x=m,y=n,X=o,Y=p,d=q;
	    ready=true;
	}
};
win wins[256];
int top=0,bottom=0;
int area(struct win w){
    for(int i=0;i<256;i++){
	if(wins[i].ready && wins[i].d>w.d){
	    if (!  (w.Y<=wins[i].y || w.y>=wins[i].Y
			|| w.X<=wins[i].x || w.x>=wins[i].X) ){
		int total=0;
		if(wins[i].x>w.x){
		    win a(w.x, w.y, wins[i].x, w.Y, w.d);
		    total+=area(a);
		}
		if(wins[i].X < w.X){
		    win b(wins[i].X, w.y, w.X, w.Y, w.d);
		    total+=area(b);
		}
		if(wins[i].y>w.y){
		    win c(max(w.x, wins[i].x), w.y, 
			    min (wins[i].X, w.X), wins[i].y, w.d);
		    total+=area(c);
		}
		if(wins[i].Y<w.Y){
		    win d(max(w.x, wins[i].x), wins[i].Y,
			min (wins[i].X, w.X), w.Y, w.d);
		    total+=area(d);
		}
		return total;
	    }
	}
    }
    return (w.Y-w.y)*(w.X-w.x);
}

int main(){
    char d,op,w;
    int x,y,X,Y;
    fin>>op;
    while(!fin.eof()){
	fin>>d>>w>>d;
	if(op=='w'){
	    fin>>x>>d>>y>>d>>X>>d>>Y>>d;
	    wins[w].ready=true;
	    wins[w].x=min(x,X);
	    wins[w].y=min(y,Y);
	    wins[w].X=max(x,X);
	    wins[w].Y=max(y,Y);
	    wins[w].d=++top;
	}
	else if(op=='t'){
	    wins[w].d=++top;//bug fix: top++ => ++top
	}
	else if(op=='b'){
	    wins[w].d=--bottom;
	}
	else if(op=='d'){
	    wins[w].ready=false;
	}
	else if(op=='s'){
	    int vis=area(wins[w]);
	    int total=(wins[w].X-wins[w].x)*(wins[w].Y-wins[w].y);
	    double res=vis*100.0/total;
	    fout<<setprecision(3)<<setiosflags(ios::fixed|ios::showpoint)
		<<res<<endl;
	}
	fin>>op;
    }
    return 0;
}
