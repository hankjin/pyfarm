/*
ID: hankjin1
LANG: C++
TASK: schlnet
*/
#include<cstdio>
#include<cstring>
#include<algorithm>
#include<iostream>

#define maxn 101

using namespace std;

int links[maxn][maxn] , n , step , top , Cnt , t = 0;
int connect[maxn][maxn];
int indegree[maxn] , outdegree[maxn] , slack[maxn*10];
bool visit[maxn];
int Belong[maxn] , LOW[maxn] , DFN[maxn];
int in[maxn];

void tarjan(int u){
    LOW[u] = DFN[u] = ++step;
    visit[u] = true;
    slack[++top] = u;
    for (int i = 1 ; i <= links[u][0] ; i++)
    {
	int v = links[u][i];
	if (!DFN[v])
	{
	    tarjan(v);
	    LOW[u] = min(LOW[u] , LOW[v]);
	}
	else
	{
	    if (visit[v])
		LOW[u] = min(LOW[u] , DFN[v]);
	}
    }
    if (LOW[u] == DFN[u])
    {
	Cnt++;
	int v;
	do
	{
	    v = slack[top--];
	    Belong[v] = Cnt;
	    visit[v] = false;
	}
	while (u!=v);
    }
}
int main(){
    freopen("schlnet.in","r",stdin);
    freopen("schlnet.out","w",stdout);
    scanf("%d",&n);
    memset(connect,false,sizeof(connect));
    for (int i = 1 ; i <= n ; i++)
    {
	int to;
	scanf("%d",&to);
	while (to){
	    links[i][++links[i][0]] = to;
	    connect[i][to] = true;
	    scanf("%d",&to);
	}
    }
    memset(DFN,0,sizeof(DFN));
    memset(LOW,0,sizeof(LOW));
    memset(visit,0,sizeof(visit));
    step = top = Cnt = 0 ;
    for (int i = 1 ; i <= n ; i++)
	if (!DFN[i]) tarjan(i);
    memset(in,0,sizeof(in));
    for (int i = 1 ; i <= n ; i++)
	for (int j = 1 ; j <= n ; j++)
	    if (Belong[i]!=Belong[j] && connect[i][j])
		in[Belong[j]]++;
    for (int i = 1 ; i <= Cnt ; i++)
	if (!in[i]) t++;
    cout<<t<<endl;
    memset(indegree,0,sizeof(indegree));
    memset(outdegree,0,sizeof(outdegree));
    for (int i = 1 ; i <= n ; i++)
	for (int j = 1 ; j <= n ; j++)
	    if (Belong[i]!=Belong[j] && connect[i][j])
	    {
		indegree[Belong[j]]++;
		outdegree[Belong[i]]++;
	    }
    int zero_in = 0 , zero_out = 0;
    for (int i = 1 ; i <= Cnt ; i++)
    {
	if (!outdegree[i]) ++zero_out;
	if (!indegree[i]) ++zero_in;
    }
    if (Cnt == 1)
	cout<<"0"<<endl;
    else
	cout<<max(zero_in,zero_out)<<endl;
    return 0;
}
