/*
ID: hankjin1
LANG: C++
TASK: tour
*/
#include <iostream>
#include <fstream>
#include <string>
#define MAXN 101
#define INF 0x7FFFFFFF

using namespace std;

ifstream fi("tour.in");
ofstream fo("tour.out");

bool adj[MAXN][MAXN];
int f[MAXN][MAXN];
string City[MAXN];
int N,ans;

inline int getnum(string &s) {
    for (int i=1;i<=N;i++)
	if (s==City[i])
	    return i;
}

void init() {
    int i,V,d1,d2;
    string C1,C2;
    fi >> N >> V;
    for (i=1;i<=N;i++)
	fi >> City[i];
    for (i=1;i<=V;i++) {
	fi >> C1 >> C2;
	d1=getnum(C1);
	d2=getnum(C2);
	adj[d2][d1]=adj[d1][d2]=true;
    }
}

void dynamic() {
    int i,j,k;
    f[1][1]=1;
    for (i=1;i<=N;i++) {
	for (j=i+1;j<=N;j++) {
	    f[i][j]=-INF;
	    for (k=1;k<j;k++) {
		if (adj[k][j] && f[i][k]>0 && f[i][k]>f[i][j])
		    f[i][j]=f[i][k];
	    }
	    f[j][i]=++f[i][j];
	    j=j;
	}
    }
}

void print() {
    int i;
    ans=1;
    for (i=1;i<N;i++)
	if (adj[i][N] && f[i][N]>ans)
	    ans=f[i][N];
    fo << ans << endl;
    fi.close();
    fo.close();
}

int main() {
    init();
    dynamic();
    print();
    return 0;
}
