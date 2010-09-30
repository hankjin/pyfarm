/*
ID: hankjin1
LANG: C++
TASK: rectbarn
*/
#include <iostream>
#include <fstream>
#include <climits>
using namespace std;

ifstream fin("rectbarn.in");
ofstream fout("rectbarn.out");
int R,C,P;
bool barn[3002][3002];
int f[2][3002], l[2][3002], r[2][3002];
int xl[3002],xr[3002];

int main(){
    fin>>R>>C>>P;
    int x,y;
    for(int i=0;i<P;i++){
	fin>>x>>y;
	barn[x][y]=true;
    }
    int now=0,last=1;
    int ans=0;
    for(int i=0;i<2;i++){//fix 1. init left and right to MAX
	for(int j=0;j<=C+1;j++)
	    l[i][j]=r[i][j]=INT_MAX;
    }
    for(int i=1;i<=R;i++){
	now^=1, last^=1;
	for(int j=0;j<=C+1;j++){
	    xl[j]=xr[j]=0;
	}
	for(int j=1;j<=C;j++){
	    if(barn[i][j]) xl[j]=0;
	    else xl[j]=1+xl[j-1];
	}
	for(int j=C;j>0;j--){
	    if(barn[i][j]) xr[j]=0;
	    else xr[j]=1+xr[j+1];
	}
	for(int j=1;j<=C;j++){
	    if(barn[i][j]){
		f[now][j]=0;
		l[now][j]=INT_MAX;
		r[now][j]=INT_MAX;
	    }
	    else{
		f[now][j]=f[last][j]+1;
		l[now][j]=min(l[last][j], xl[j]);
		r[now][j]=min(r[last][j], xr[j]); //fix3. l->r
	    }
	    ans = max(ans, f[now][j]*(l[now][j]+r[now][j]-1) ); //fix2. -1
//	    cout<<i<<","<<j<<":"<<ans<<endl;
	}
    }
    fout<<ans<<endl;
    return 0;
}

