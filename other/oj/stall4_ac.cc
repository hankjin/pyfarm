/*
ID: hankjin1
LANG: C++
TASK: stall4
*/
#include <fstream>
#include <iostream> 
using namespace std; 
#define MAX 400
long n,m,match;
long adjl[MAX][MAX];
long mat[MAX];
bool used[MAX];
ifstream fin("stall4.in");
ofstream fout("stall4.out");

void readfile() {
    fin>>n>>m;
    long tmp,stall;
    for(int i=1;i<=n;i++){
	fin>>tmp;
	for(int j=0;j<tmp;j++){
	    fin>>stall;
	    adjl[i][ ++adjl[i][0] ]=stall+n;
	}
    }
    match=0;
}
bool crosspath(long k) {
    for (long i=1;i<=adjl[k][0];i++) {
	long j=adjl[k][i];
	if (!used[j]) {
	    used[j]=true;
	    if (mat[j]==0 || crosspath(mat[j])) {
		mat[j]=k;
		return true;
	    }
	}
    }
    return false;
}
void hungary() {
    for (long i=1;i<=m;i++) {
	fill_n(used, sizeof(used), 0);
	if (crosspath(i))
	    match++;
    }
}
int main() {
    readfile();
    hungary();
    fout<<match<<endl;
    return 0;
}
