/**
ID: hankjin1
LANG: C++
TASK: camelot
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
 
#define maxColumns 26
#define maxRows 30
#define maxint 9999999
 
using namespace std;
 
ifstream fin("camelot.in");
ofstream fout("camelot.out");
 
const int knightStep[8][2]={{1,2},{2,1},{2,-1},{1,-2},{-1,-2},{-2,-1},{-2,1},{-1,2}};
 
int step[maxColumns][maxRows][maxColumns][maxRows];
bool hash[maxColumns][maxRows];
int king[2],knight[maxColumns*maxRows+1][2];
int r,c,totKnight=-1;
int ans=maxint;
 
void bfs(int x,int y)
{
	int queue[maxColumns*maxRows][2];
	int tail=-1,head=0;
	for(int i=0;i<c;i++)
	for(int j=0;j<r;j++)
	{
		hash[i][j]=false; step[x][y][i][j]=maxint;
	}
	queue[0][0]=x; queue[0][1]=y; step[x][y][x][y]=0; hash[x][y]=true;
 
	while(tail<head)
	{
		tail++;
		for(int i=0;i<8;i++)
		{
			int tx=queue[tail][0]+knightStep[i][0];
			int ty=queue[tail][1]+knightStep[i][1];
			if((tx>=0)&&(tx<c)&&(ty>=0)&&(ty<r)&&(!hash[tx][ty]))
			{
				step[x][y][tx][ty]=step[x][y][queue[tail][0]][queue[tail][1]]+1;
				queue[++head][0]=tx; queue[head][1]=ty;
				hash[tx][ty]=true;
			}
		}
	}
}
 
int tryit(int x,int y)
{
	int sum=0;
	for(int k=0;k<totKnight;k++) sum+=step[knight[k][0]][knight[k][1]][x][y];
 
	int ans=sum+max(abs(king[0]-x),abs(king[1]-y));
	for(int tx=max(0,king[0]-1);tx<=min(c-1,king[0]+1);tx++)
	for(int ty=max(0,king[1]-1);ty<=min(r-1,king[1]+1);ty++)
	for(int k=0;k<totKnight;k++)
		ans=min(ans,sum-step[knight[k][0]][knight[k][1]][x][y]+step[knight[k][0]][knight[k][1]][tx][ty]+step[tx][ty][x][y]+max(abs(king[0]-tx),abs(king[1]-ty)));
 
	return ans;
}
 
void calcStep()
{
	for(int i=0;i<c;i++)
	for(int j=0;j<r;j++)
		bfs(i,j);
}
 
void init()
{
	fin>>r>>c;
	char row; int column;
	fin>>row>>column; 
	king[0]=static_cast<int>(row-'A'); 
	king[1]=column-1;
 
	while(fin)
	{
		fin>>row>>column;
		knight[++totKnight][0]=static_cast<int>(row-'A');
		knight[totKnight][1]=column-1;
	}
}
 
void answer()
{
	for(int i=0;i<c;i++)
	for(int j=0;j<r;j++)
		ans=min(ans,tryit(i,j));
	fout<<ans<<endl;
	//cout<<ans<<endl;
}
 
int main()
{
	init();
	calcStep();
	answer();
}
