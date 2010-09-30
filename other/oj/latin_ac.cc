/*
ID: hankjin1
LANG: C++
TASK: latin
*/
#include <iostream>
#include <fstream>
#include <cstring>
#define maxn 9

using namespace std;

ifstream fi("latin.in");
ofstream fo("latin.out");

int n,i,idx;
char a[maxn];
bool v[maxn],row[maxn][maxn],col[maxn][maxn];
int cnt[maxn];
unsigned long long total;

void calidx() {
    char i,l,t;
    idx=2;
    memset(v,0,sizeof(v));
    for (i=1;i<=n;i++)
	if (!v[i]) {
	    l=0;t=i;
	    do {
		v[t]=true;
		t=a[t];
		l++;
	    }while (!v[t]);
	    if (l>idx) idx=l;
	}
}

void search(char x,char y) {
    char i;
    for (i=1;i<=n;i++)
	if (row[x][i] && col[y][i]) {
	    if (x==2) {
		a[y]=i;
		if (y==n) {
		    calidx();
		    if (cnt[idx]>0) {
			total+=cnt[idx];
			return;
		    }
		}
	    }
	    row[x][i]=false;
	    col[y][i]=false;
	    if (y==n) {
		if (x==n-1) {
		    cnt[idx]++;
		    total++;
		}
		else
		    search(x+1,2);
	    }
	    else
		search(x,y+1);
	    row[x][i]=true;
	    col[y][i]=true;
	}
}



int main()
{
    fi >> n;
    if (n==2)
	total=1;
    else {
	memset(row,1,sizeof(row));
	memset(col,1,sizeof(col));
	memset(cnt,0,sizeof(cnt));
	for (i=2;i<n;i++) row[i][i]=false;
	for (i=1;i<=n;i++) col[i][i]=false;
	total=0;
	a[1]=2;
	search(2,2);
	for (i=2;i<n;i++)
	    total*=i;
    }
    fo << total << endl;
    fi.close();
    fo.close();
    return 0;
}
