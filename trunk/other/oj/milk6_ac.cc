/*
ID: hankjin1
LANG: C++
TASK: milk6
*/
#include <iostream>
#include <string.h>
using namespace std;
#define int64 long long 
int64 n,m;
struct edge
{
    int64 x,y,f;
}e[2001];
int64 c[40][40];
int64 c2[40][40];
int64 ans[2000];
void init() {
    freopen("milk6.in","r",stdin);
    freopen("milk6.out","w",stdout);
    cin>>n>>m;
    for(int i=1;i<=m;i++) {
	int64 x,y,z;
	cin>>x>>y>>z;
	e[i].x=x;
	e[i].y=y;
	e[i].f=z*1001+1;
	c[x][y]+=z*1001+1;
    }
}
class NETFLOW {
    public:
	int64 vh[100],h[100];
	int64 ans;
	int64 flow;
	bool found;
	void aug(int x) {
	    if(x==n) {
		found=true;
		ans+=flow;
		return;
	    }
	    int64  minh=n-1,old=flow,i;
	    for(i=1;i<=n;i++)
		if(c2[x][i]>0) {
		    if(h[i]+1==h[x]) {
			if(c2[x][i]<flow)
			    flow=c2[x][i];
			aug(i);
			if(h[1]>=n) return;
			if(found) break;
		    }
		    if(minh>h[i])
			minh=h[i];
		}
	    if(found) {
		c2[x][i]-=flow;
		c2[i][x]+=flow;
	    }
	    else {
		vh[h[x]]--;
		if(vh[h[x]]==0)
		    h[1]=n;
		h[x]=minh+1;
		vh[h[x]]++;
	    }
	}
	int64 getmax() {
	    memset(h,0,sizeof(h));
	    memset(vh,0,sizeof(vh));
	    h[0]=n;
	    ans=0;
	    while(h[1]<n) {
		flow=0x7FFFFFFF;
		found=false;
		aug(1);
	    }
	    return ans;
	}
}maxflow;

main() {
    init();
    memcpy(c2,c,sizeof(c2));
    int64 max=maxflow.getmax();
    cout<<max/1001<<' ';

    for(int64 i=1;i<=m;i++) {
	if(max==0) break;
	memcpy(c2,c,sizeof(c2));
	c2[e[i].x][e[i].y]-=e[i].f;
	int64 now=maxflow.getmax();
	if(now+e[i].f==max) {
	    max-=e[i].f;
	    c[e[i].x][e[i].y]-=e[i].f;
	    ans[0]++;
	    ans[ans[0]]=i;
	}
    }
    cout<<ans[0]<<endl;
    for(int64 i=1;i<=ans[0];i++)
	cout<<ans[i]<<endl;
    return 0;
}
