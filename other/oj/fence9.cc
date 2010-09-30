/**
ID: hankjin1
LANG: C++
TASK: fence9
*/
#include <iostream>
#include <fstream>
using namespace std;

int n,m,p;
int lnode[32000];
int rnode[32000];
inline bool down(int a, int b, int x, int y){
    return a*y-b*x > 0;
}
inline bool up(int a, int b, int x, int y){
    return a*y-b*x < 0;
}
int main(){
    ifstream fin("fence9.in");
    ofstream fout("fence9.out");
    fin>>n>>m>>p;
    int x = 0, total = 0;
    for(int i=1;i<=n;i++){//calculate 0-n
	while(down(i, x+1, n, m))
	    x++;
	lnode[i] = x;
    }
    x = 0;
    if(p>n){//calculate n-p
	for(int i=p-1;i>n;i--){
	    while(down(i-n, x+1-m, p-n, 0-m))
		x++;
	    rnode[i]=x;
	}
    }
    else if(p<n){
	for(int i=p+1;i<n;i++){
	    while(!up(i-p, x+1, n-p, m-0))
		x++;
	    rnode[i]=min(lnode[i],x);
	}
    }
    for(int i=1;i<n;i++)//left part
	total+=lnode[i];
    int l=min(n,p);
    int r = max(n,p);
    for(int i=l+1;i<r;i++){//right part
	if(p>n)
	    total += rnode[i];
	else
	    total -= rnode[i];
    }
    if(p>n)//mid part
	total += lnode[n];
    fout<<total<<endl;
    return 0;
}

