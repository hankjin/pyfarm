/**
ID: hankjin1
LANG: C++
TASK: ttwo
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

ifstream fin("ttwo.in");
ofstream fout("ttwo.out");

char grid[10][10];
int cx,cy,fx,fy,cdir,fdir;
int dir[4][2]={
    {-1,0},
    {0,1},
    {1,0},
    {0,-1}
};
int main(){
    for(int i=0;i<10;i++){
	for(int j=0;j<10;j++){
	    fin>>grid[i][j];
	    if(grid[i][j]=='C'){
		grid[i][j]='.';
		cx=i;
		cy=j;
	    }
	    else if(grid[i][j]=='F'){
		grid[i][j]='.';
		fx=i;
		fy=j;
	    }
	}
    }
    cdir = fdir = 0;
    int k;
    int newx,newy;
    for(k=1;k<160000;k++){
	newx=cx+dir[cdir][0];
	newy=cy+dir[cdir][1];
	if(newx>=0 && newy>=0 
		&& newx<10 && newy<10
		&& grid[newx][newy]=='.'){
	    cx=newx;
	    cy=newy;
	}
	else{
	    cdir = (cdir+1)%4;
	}
	newx=fx+dir[fdir][0];
	newy=fy+dir[fdir][1];
	if(newx>=0 && newy>=0 
		&& newx<10 && newy<10
		&& grid[newx][newy]=='.'){
	    fx=newx;
	    fy=newy;
	}
	else{
	    fdir = (fdir+1)%4;
	}

	if(cx==fx && cy==fy){
	    fout<<k<<endl;
	    break;
	}
    }
    if(k==160000){
	fout<<0<<endl;
    }

    return 0;
}
