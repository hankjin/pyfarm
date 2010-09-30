/*
ID: hankjin1
LANG: C++
TASK: wissqu
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;
ifstream inf("wissqu.in");
ofstream ouf("wissqu.out");
struct move_t{
    int t,r,c;   
} ans[17],tans[17];
bool u[5][5];
int num[5],map[6][6],sum=0;
void dfs(int d,int ty){
    if (d>=16) {
	if (!sum) 
	    memcpy(ans,tans,sizeof(ans));    
	sum++;
	return;
    } 
    int i,j,k;
    for (i=1;i<=4;i++)
	for (j=1;j<=4;j++)
	    if (!u[i][j] && map[i-1][j-1]!=ty && map[i-1][j]!=ty && map[i-1][j+1]!=ty
		    && map[i][j-1]!=ty && map[i][j]!=ty && map[i][j+1]!=ty 
		    && map[i+1][j-1]!=ty && map[i+1][j]!=ty && map[i+1][j+1]!=ty)
	    {
		int tmp=map[i][j];
		map[i][j]=ty;num[ty]--;
		u[i][j]=true;tans[d+1].t=ty;
		tans[d+1].r=i;tans[d+1].c=j;
		if (d==15) dfs(d+1,0);
		else
		    for (k=0;k<5;k++)
			if (num[k])
			    dfs(d+1,k);
		map[i][j]=tmp;num[ty]++;
		u[i][j]=false;
	    }
}

int main(){
    int i,j;
    memset(map,255,sizeof(map));
    ans[1].t=5;
    for (i=1;i<=4;i++) {
	for (j=1;j<=4;j++) {
	    char ch;
	    inf >> ch;
	    map[i][j]=ch-65;
	    num[map[i][j]]++;
	}
	inf.ignore(255,'\n');
    }   
    num[3]++;num[2]--;
    dfs(0,3);
    for (i=1;i<=16;i++)
	ouf << (char)(ans[i].t+65) << ' ' << ans[i].r << ' ' << ans[i].c << endl;
    ouf << sum << endl;    
}
