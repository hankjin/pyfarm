/**
ID: hankjin1
LANG: C++
TASK: snail
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

ifstream fin("snail.in");
ofstream fout("snail.out");

int N,B;
int grid[120][120];//0:empty 1:barriar x:way
struct d{
    int x,y;
}dir[4]={ {-1,0}, {1,0}, {0,-1}, {0, 1} };
int dfs(int x, int y, int pos){
    int nx,ny,steps;
    int res = 0;
    for(int i=0;i<4;i++){
	nx = x+dir[i].x, ny=y+dir[i].y;
	if(nx<0||ny<0||nx==N||ny==N||grid[nx][ny]>0)
	    continue;
	steps=0;
	//go
	do{
	    grid[nx][ny]=pos;
	    steps++;
	    nx+=dir[i].x;
	    ny+=dir[i].y;
	}while(nx>=0&&ny>=0&&nx<N&&ny<N&&grid[nx][ny]==0);
//	cout<<"From:"<<x<<","<<y<<"To:"<<nx<<","<<ny<<endl;
	//try next
	if(nx>=0 && ny>=0 && nx<N && ny<N &&grid[nx][ny]>1){//big fix.!!!
	    res = max(res, steps);
	}
	else{
	    res = max(res, steps+dfs(nx-dir[i].x,ny-dir[i].y,pos+1));
	}
	//go back
	nx = x+dir[i].x, ny=y+dir[i].y;
	do{
	    grid[nx][ny]=0;
	    nx+=dir[i].x;
	    ny+=dir[i].y;
	}while(nx>=0&&ny>=0&&nx<N&&ny<N&&grid[nx][ny]==pos);
//	cout<<"Back:"<<x<<","<<y<<"To:"<<nx<<","<<ny<<endl;
    }
    return res;
}
int main(){
    fin>>N>>B;
    //bug fix2, A2 is not char A, but char A and because A10!!!
    char y;
    int x;
    for(int i=0;i<B;i++){
	fin>>y>>x;	
	grid[x-1][y-'A']=1;
    }
    grid[0][0]=2;
    int ans=dfs(0,0,3)+1;
    fout<<ans<<endl;
    return 0;
}
