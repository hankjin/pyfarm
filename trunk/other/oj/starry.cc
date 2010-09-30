/*
ID: hankjin1
LANG: C++
TASK: starry
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

int W,H;
int sky[100][100];
struct shape{
    int minx,miny,maxx,maxy;
    int w,h;
    int num;
    char c;
    int mark;
}shapes[10000];
int sd[8][100][100];
//this is the most confusing part
void mksd(struct shape s){
    //create 8 shapeds
    for(int i=0;i<=s.h;i++){
	for(int j=0;j<=s.w;j++){
	    sd[0][i][j]=sky[s.minx+i][s.miny+j];//same
	    sd[1][i][s.w-j]=sd[0][i][j];//mirrow of 0
	    sd[2][s.h-i][s.w-j]=sd[0][i][j];
	    sd[4][j][s.h-i]=sd[0][i][j];
	}
    }
    for(int i=0;i<=s.h;i++){//fix 1, split them out
	for(int j=0;j<=s.w;j++){
	    sd[3][i][s.w-j]=sd[2][i][j];//mirrow of 2
	}
    }
    for(int i=0;i<=s.w;i++){//fix 4, w is h, h is w
	for(int j=0;j<=s.h;j++){
	    sd[5][i][s.h-j]=sd[4][i][j];//mirrow of 4
	    sd[6][s.w-i][s.h-j]=sd[4][i][j];
	}
    }
    for(int i=0;i<=s.w;i++){
	for(int j=0;j<=s.h;j++){
	    sd[7][i][s.h-j]=sd[6][i][j];//mirrow of 6
	}
    }
    /*
    for(int k=0;k<8;k++){
	cout<<k<<endl;
	for(int i=0;i<=4;i++){
	    for(int j=0;j<=4;j++){
		if(sd[k][i][j]==0)
		    cout<<0;
		else
		    cout<<'x';
	    }
	    cout<<endl;
	}
	cout<<endl;
    }
    //*/
}
int dir[8][2]={{-1,-1},{0,-1},{1,-1},//top
    {-1,0},{1,0},//same line
    {-1,1},{0,1},{1,1}//bottom
};
int floodfill(int x,int y, int v){
    sky[x][y] = 100+v;
    shapes[v].minx = min(shapes[v].minx, x);
    shapes[v].miny = min(shapes[v].miny, y);
    shapes[v].maxx = max(shapes[v].maxx, x);
    shapes[v].maxy = max(shapes[v].maxy, y);
    int res = 1;
    for(int i=0;i<8;i++){
	int nx = x+dir[i][0],
	    ny = y+dir[i][1];
	if(nx>=0 && nx<H && ny>=0 && ny<W && sky[nx][ny]==1)
	    res+=floodfill(nx,ny,v);
    }
    return res;
}
inline bool issame(struct shape &s1, struct shape &s2){
    if(s1.num != s2.num)
	return false;
    int from=0,to=0;
    if(s1.w==s2.w && s1.h==s2.h){
    }
    else if(s1.w==s2.h && s1.h==s2.w){
    }
    else{
	return false;
    }
    for(int k=0;k<8;k++){
	bool same=true;
	for(int i=0;i<=s2.h && same;i++){
	    for(int j=0;j<=s2.w;j++){
		if(sky[s2.minx+i][s2.miny+j] == s2.mark+100
			&& sd[k][i][j] !=s1.mark+100){
		    same = false;
		    break;
		}
	    }
	}
	if(same){
	    return true;
	}
    }
    return false;
}
int main(){
    ifstream fin("starry.in");
    ofstream fout("starry.out");
    fin>>W>>H;
    char tmp;
    for(int i=0;i<H;i++){
	for(int j=0;j<W;j++){
	    fin>>tmp;
	    sky[i][j]= tmp=='0'?0:1;
	}
    }
    int shapeN = 0;
    for(int i=0;i<H;i++){
	for(int j=0;j<W;j++){
	    if(sky[i][j]==1){
		shapes[shapeN].minx=shapes[shapeN].miny=100;
		shapes[shapeN].num=floodfill(i,j,shapeN);//flood fill
		shapes[shapeN].mark=shapeN;
		shapes[shapeN].h=shapes[shapeN].maxx-shapes[shapeN].minx; 
		shapes[shapeN].w=shapes[shapeN].maxy-shapes[shapeN].miny; 

		shapeN++;
	    }
	}
    }
    char c = 'a';
    //give alpha to every shape
    for(int i=0;i<shapeN;i++){
	if(shapes[i].c != 0)
	    continue;
	shapes[i].c=c;
	mksd(shapes[i]);
	for(int j=i+1;j<shapeN;j++){
	    if(issame(shapes[i],shapes[j])){
		shapes[j].c=c;
	    }
	}
	c++;
    }
    //output shapes
    for(int i=0;i<H;i++){
	for(int j=0;j<W;j++){
	    if(sky[i][j]==0)
		fout<<0;
	    else
		fout<<shapes[sky[i][j]-100].c;
	}
	fout<<endl;
    }
    return 0;
}

