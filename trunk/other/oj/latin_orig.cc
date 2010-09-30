/*
ID: hankjin1
LANG: C++
TASK: latin
*/
#include <iostream>
#include <fstream>
using namespace std;

//note: case 6 passed
ifstream fin("latin.in");
ofstream fout("latin.out");
int N;
int mask;
int dict[5040][7];
int row[7];
int res;
void solve(int x,int y,int col){
    if(x==N-1 && y==N-1){
	res++;
	return ;
    }
    int nx= y==N-1? x+1 : x;
    int ny= y==N-1? 0 : y+1;
    int t=mask&(~ (row[y]|col) );
    int raw=row[y];
    while(t>0){
	int v = t&(-t);
	row[y]=raw|v;
	if(ny==0)
	    solve(nx,ny, 0);
	else
	    solve(nx,ny, col|v);
	row[y]=raw;
	t-=v;
    }
}

int main(){
    fin>>N;
    mask=(1<<N)-1;
    for(int i=0;i<N;i++)
	row[i]=1<<i;
    res=0;
    solve(1,0,0);
    fout<<res<<endl;
    return 0;
}
