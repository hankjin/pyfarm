/*
ID: hankjin1
LANG: C++
TASK: rect1
*/
#include <iostream>
#include <fstream>
using namespace std;

int A,B,N;
struct area{
    int llx;
    int lly;
    int urx;
    int ury;
    int color;
    bool walked;
}areas[100000];
int areaN;
int colors[2501];
void addarea(int x1, int y1, int x2, int y2, int c){
    struct area tmp={x1,y1,x2,y2,c};
    areas[areaN++]=tmp;
}
/*
+---------+
|  | 4 |  |
|  +---+  |
|  | x |  |
|1 |   | 3|
|  +---+  |
|  | 2 |  |
+---------+
*/
void add(int x1, int y1, int x2, int y2, int c){
    int oldSize = areaN;
    addarea(x1,y1,x2,y2,c);
    for(int i=0;i<oldSize;i++){
	if(areas[i].walked)
	    continue;
	//that is the point!!!
	if(x1>=areas[i].urx || x2<=areas[i].llx
		||y1>=areas[i].ury || y2<=areas[i].lly)
	    continue;
	int lx=areas[i].llx>x1 ? areas[i].llx : x1;
	int rx=areas[i].urx<x2 ? areas[i].urx : x2;

	if(x1>areas[i].llx && x1<areas[i].urx){//1
	    addarea(areas[i].llx, areas[i].lly, x1, areas[i].ury, areas[i].color);
	}
	if(y1>areas[i].lly && y1<areas[i].ury){//2
	    addarea(lx, areas[i].lly, rx, y1, areas[i].color);
	}
	if(x2>areas[i].llx && x2<areas[i].urx){//3
	    addarea(x2, areas[i].lly, areas[i].urx, areas[i].ury,areas[i].color);
	}
	if(y2>areas[i].lly && y2<areas[i].ury){//4
	    addarea(lx, y2, rx, areas[i].ury, areas[i].color);
	}
	areas[i].walked=true;
    }
}

int calc(int i){
    return (areas[i].urx-areas[i].llx)*(areas[i].ury-areas[i].lly);
}
int main(){
    ifstream fin("rect1.in");
    ofstream fout("rect1.out");
    fin>>A>>B>>N;
    addarea(0,0,A,B,1);
    int llx,lly,urx,ury,color;
    for(int i=0;i<N;i++){
	fin>>llx>>lly>>urx>>ury>>color;
	add(llx,lly,urx,ury,color);
    }
    for(int i=0;i<areaN;i++){
	if(areas[i].walked){
	    continue;
	}
#ifdef DEBUG
	cout<<"("<<areas[i].llx<<","<<areas[i].lly
	    <<"),("<<areas[i].urx<<","<<areas[i].ury<<endl;
#endif

	colors[areas[i].color]+=calc(i);
    }
    for(int i=1;i<=2500;i++){
	if(colors[i]!=0)
	    fout<<i<<" "<<colors[i]<<endl;
    }
    return 0;
}
