/**
ID: hankjin1
LANG: C++
TASK: checker
*/
//hint4: reduce half by rotate
//
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
using namespace std;

int N;
int found;
int pos[13];
bool used[13];//used rows
bool xadd[26];//used left-down to right-up
bool xsub[26];//used left-top to right-down
ifstream fin("checker.in");
ofstream fout("checker.out");
int hist[13];

void dfs(int x){
    if(x==N){
	if(found<3){
	    int i;
	    for(i=0;i<N-1;i++){
		fout<<(1+pos[i])<<" ";
		hist[i]=pos[i];
	    }
	    fout<<(1+pos[i])<<endl;
	    hist[i]=pos[i];
	}
	found++;
	return ;
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
	dfs(x+1);
	used[i]=false;
	xadd[x+i]=false;
	xsub[13+x-i]=false;
    }
}
int place(int n){
    used[n]=true;
    xadd[0+n]=true;
    xsub[13+0-n]=true;
    pos[0]=n;
    dfs(1);
    used[n]=false;
    xadd[0+n]=false;
    xsub[13+0-n]=false;
}
int main(){
    fin>>N;
    fill_n(used, 0, 13);
    fill_n(xadd, 0, 26);
    fill_n(xsub, 0, 26);
    found = 0;
    int i, top, sum;
    for(i=0;i<N/2;i++){
	place(i);
    }
    top = found;
    if(N%2!=0)
	place(i);
    if(found<3){
	for(i=0;i<N-1;i++){
	    fout<<(N-hist[i])<<" ";
	}
	fout<<(N-hist[i])<<endl;
    }
    sum = (found-top)+2*top;
    fout<<sum<<endl;

    return 0;
}
