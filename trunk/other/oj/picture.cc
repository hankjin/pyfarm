/*
ID: hankjin1
LANG: C++
TASK: picture
*/
#include<cstdio>
#include<cstring>
#include<algorithm>
using namespace std;
struct INTERVAL{
    int x,y1,y2,add;   
    INTERVAL(int ix=0,int iy1=0,int iy2=0,int iadd=0):
	x(ix),y1(iy1),y2(iy2),add(iadd)  {}
};
struct NODE{
    int c,line;   
    bool lb,rb; 
};
NODE tr[1<<16];
INTERVAL reg[10010];
int cord[5010][4];
int n,my,ans;
int ll,rr,add;
void input()
{   
    scanf("%d",&n);      
    my=-1;
    for(int i=0;i<n;++i)
    {
	scanf("%d%d%d%d",cord[i],cord[i]+1,cord[i]+2,cord[i]+3);                                   
    }
}
bool cmp(const INTERVAL &p1,const INTERVAL &p2)
{
    return p1.x<p2.x;     
}
void insert(int x,int l,int r)
{
    if(ll>r || rr<l ) return ;
    if(ll<=l && r<=rr ) {
	if(add>0) {     
	    tr[x].c++;
	    tr[x].line=1;
	    tr[x].lb=tr[x].rb=1;
	}
	else{
	    tr[x].c--;
	    if(tr[x].c==0)  
		
	    {
		if(l<r) 
		{
		    tr[x].lb=tr[x*2].lb;
		    tr[x].rb=tr[x*2+1].rb;
		    tr[x].line=tr[x*2].line+tr[x*2+1].line-tr[x*2].rb*tr[x*2+1].lb;
		} else {   
		    tr[x].lb=tr[x].rb=0;
		    tr[x].line=0;
		}
	    }
	}
    }
    else {
	insert(x*2,l,(l+r)/2);                      
	insert(x*2+1,(l+r)/2+1,r);           
	if(!tr[x].c) {   
	    
	    tr[x].lb=tr[x*2].lb;
	    tr[x].rb=tr[x*2+1].rb;
	    tr[x].line=tr[x*2].line+tr[x*2+1].line-tr[x*2].rb*tr[x*2+1].lb;
	}                                    
    }            
}
void work()
{       
    sort(reg,reg+2*n,cmp);
    ll=reg[0].y1;
    rr=reg[0].y2;
    add=reg[0].add;
    insert(1,0,my);
    for(int i=1;i<2*n;++i)
    {
	ans+=(reg[i].x-reg[i-1].x)*2*tr[1].line;
	ll=reg[i].y1;
	rr=reg[i].y2;
	add=reg[i].add;
	insert(1,0,my);
    }       
}
void solve()
{
    int x1,y1,x2,y2;   
    my=-1;
    memset(tr,0,sizeof(tr));
    for(int i=0;i<n;++i)
    {
	x1=cord[i][0];
	y1=cord[i][1];
	x2=cord[i][2];
	y2=cord[i][3];
	y1+=10000;
	y2+=10000;
	if(y1>my) my=y1;
	if(y2>my) my=y2;
	reg[i]=INTERVAL(x1,y1,y2-1,1);
	reg[i+n]=INTERVAL(x2,y1,y2-1,-1);             
    }
    work();   
}
int main()
{
    freopen("picture.in","r",stdin);   
    freopen("picture.out","w",stdout);
    input();
    ans=0;
    solve();                    
    for(int i=0;i<n;++i)
    {  
	swap(cord[i][0],cord[i][1]);
	swap(cord[i][2],cord[i][3]);
    }     
    solve();
    printf("%d\n",ans);
    return 0;
}
