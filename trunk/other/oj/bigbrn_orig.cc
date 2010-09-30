/*
ID: hankjin1
LANG: C++
TASK: bigbrn
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

ifstream fin("bigbrn.in");
ofstream fout("bigbrn.out");

int N,T;
struct tree{
    int x,y;
}trees[10000];

typedef struct _area{
    int x,y,X,Y;
    public:
    _area(int a,int b, int c,int d){
	x=a,y=b,X=c,Y=d;
    }
}area;
int res;

//note: orig version pass 4 cases
//note: after use if to cut, pass 13 cases
void split(area w,int ti){
    for(int i=ti;i<T;i++){
	if(trees[i].x>w.x && trees[i].x<w.X
		&&trees[i].y>w.y && trees[i].y<w.Y){
	    if(trees[i].x-w.x>res && w.Y-w.y>res){
		area a(w.x, w.y, trees[i].x, w.Y);
		split(a,ti+1);
	    }
	    if(w.X-w.x>res && trees[i].y-w.y>res){
		area b(w.x, w.y, w.X, trees[i].y);
		split(b,ti+1);
	    }
	    if(w.X-w.x>res && w.Y-trees[i].y>res){
		area c(w.x, trees[i].y, w.X, w.Y);
		split(c,ti+1);
	    }
	    if(w.X-trees[i].x>res &&w.Y-w.y>res){
		area d(trees[i].x, w.y, w.X, w.Y);
		split(d,ti+1);
	    }
	    return ;
	}
    }
    res = max(res, min(w.X-w.x, w.Y-w.y));
}
int main(){
    fin>>N>>T;
    int x,y;
    for(int i=0;i<T;i++){
	fin>>x>>y;
	trees[i].x=x-1;
	trees[i].y=y-1;
    }
    area w(-1,-1,N,N);
    res=0;
    split(w,0);
    fout<<res-1<<endl;
    return 0;
}
