/**
ID: hankjin1
LANG: C++
TASK: checker
*/
//note: use nq to calculate possibilities
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
using namespace std;

int N;
int found;
int status;
int pos[13];
bool used[13];//used rows
bool xadd[26];//used left-down to right-up
bool xsub[26];//used left-top to right-down
ifstream fin("checker.in");
ofstream fout("checker.out");
int hist[13];

bool dfs(int x){
    if(found == 3)
	return true;
    if(x==N){
	found++;
	int i;
	for(i=0;i<N-1;i++){
	    fout<<(1+pos[i])<<" ";
	    hist[i]=pos[i];
	}
	fout<<(1+pos[i])<<endl;
	hist[i]=pos[i];
	return false;
    }
    int i;
    for(i=0;i<N;i++){
	if(used[i])
	    continue;
	if(xadd[x+i])
	    continue;
	if(xsub[13+x-i])
	    continue;
	used[i]=true;
	xadd[x+i]=true;
	xsub[13+x-i]=true;
	pos[x]=i;
	if(dfs(x+1))
	    return true;;
	used[i]=false;
	xadd[x+i]=false;
	xsub[13+x-i]=false;
    }
    return false;
}
void nq(int row, int ld, int rd){
    if(row != status){
	int pos = status & (~(row|ld|rd));
	int p;
	while(pos!=0){
	    p = pos&(-pos);
	    pos -= p;
	    nq(row+p, (ld+p)<<1, (rd+p)>>1);
	}
    }
    else
	found++;
}
int main(){
    fin>>N;
    fill_n(used, 0, 13);
    fill_n(xadd, 0, 26);
    fill_n(xsub, 0, 26);
    found = 0;
    dfs(0);
    found = 0;
    status=(1<<N)-1;
    nq(0,0,0);
    fout<<found<<endl;
    return 0;
}
