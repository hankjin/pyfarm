#include<iostream>
#include <fstream>
#define OO 10000000
using namespace std;

ifstream fin("rectbarn.in");
ofstream fout("rectbarn.out");
int f[2][3001],l[2][3001],r[2][3001];
bool broke[3001][3001];
int main() {
    int i,j,R,C,P,x,y,last,now,ans;
    int left[3001],right[3002];

    fin>>R>>C>>P;
    for (i=0;i<P;i++) {
	fin>>x>>y;
	broke[y][x]=true;//郁闷...改成broke[x][y]就错了
    }
    ans=1;now=0;last=1;
    for (i=1;i<=C;i++) {
	now^=1;last^=1;
	left[0]=0;right[R+1]=0;
	for (j=1;j<=R;j++)
	    if (broke[i][j]) left[j]=0;
	    else left[j]=left[j-1]+1;
	for (j=R;j>0;j--)
	    if (broke[i][j]) right[j]=0;
	    else right[j]=right[j+1]+1;
	for (j=1;j<=R;j++) {
	    if (broke[i][j]) {
		f[now][j]=0;l[now][j]=OO;r[now][j]=OO;
	    }
	    else {
		f[now][j]=f[last][j]+1;
		l[now][j]=min(l[last][j],left[j]);
		r[now][j]=min(r[last][j],right[j]);
	    }
	    ans=max(ans,f[now][j]*(l[now][j]+r[now][j]-1));
	}
    }
    if (P==0) fout<<R*C<<endl;//特殊判断没有障碍的情况
    else fout<<ans<<endl;
    return 0;
}
