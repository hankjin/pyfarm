/*
ID: hankjin1
LANG: C++
TASK: holstein
*/
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;
// note: C(n,x)=C(n,n-x) cancel half the calculation
// note: if n>2m and n-x(0<x<m) failed, then x-m all failed
//#define DEBUG

ifstream fin("holstein.in");
ofstream fout("holstein.out");

int V;
int vs[25];
int G;
int gs[15][25];
bool cs[15];
int total[25];

int oks[15];

int revs[15];
int revN = 0;

bool isok(int n){
    int i,j;
    int status = 0;
    bool ok = true;
    bool revok = true;
    int sums[25];
    for(i=0;i<V;i++){
	sums[i]=0;
	for(j=0;j<n;j++){
	    sums[i]+=gs[oks[j]][i];
	}
	if(sums[i]<vs[i])
	    ok = false;
	if(total[i]-sums[i] < vs[i])
	    revok = false;
	if(!ok && !revok)
	    break;
    }
    if(ok)
	return true;
    if(revok){
	revok=false;
	if(n==revN){
	    for(i=0;i<G;i++){
		if(revs[i] && !cs[i]){
		    revok=true;
		    break;
		}
		else if(cs[i] && !revs[i]){
		    revok=false;
		    break;
		}
	    }
	}
	else if(n>revN){
	    revok=true;
	}
	if(revok){
#ifdef DEBUG
	    cout<<"Got a REV:"<<n<<endl;
#endif
	    revN=n;
	    for(i=0;i<G;i++){
		revs[i]=cs[i];
	    }
	}
    }
    return false;
}
bool solve(int n, int x){
    if(x==n){
	if(isok(n)){
	    return true;
	}
	else
	    return false;
    }
    int i,j;
    for(i=0;i<G;i++){
	if(cs[i])
	    continue;

	cs[i]=true;
	oks[x]=i;
	if(solve(n, x+1))
	    return true;
	cs[i]=false;
    }
    return false;
}
int main(){
    int i,j;
    fin>>V;
    for(i=0;i<V;i++){
	fin>>vs[i];
    }
    fin>>G;
    fill_n(total,0,25);
    for(i=0;i<G;i++){
	for(j=0;j<V;j++){
	    fin>>gs[i][j];
	    total[j]+=gs[i][j];
	}
    }
    fill_n(cs,0,15);
    bool found=false;
    for(i=1;i<=G/2;i++){
#ifdef DEBUG
	cout<<"Combine"<<i<<endl;
#endif
	if((revN+1) != i){
	    break;
	}
	if(solve(i,0)){
	    found=true;
	    break;
	}
    }
    if(found){
	fout<<i;
	for(i=0;i<G;i++){
	    if(cs[i])
		fout<<" "<<(i+1);
	}
    }
    else{
	fout<<(G-revN);
	for(i=0;i<G;i++){
	    if(!revs[i])
		fout<<" "<<(i+1);
	}
    }
    fout<<endl;
    return 0;
}
