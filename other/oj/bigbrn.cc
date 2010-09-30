/*
ID: hankjin1
LANG: C++
TASK: bigbrn
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
using namespace std;
/* ab
 * cd
 * =>DP:  d=min(a,b,c)+1
 */

ifstream fin("bigbrn.in");
ofstream fout("bigbrn.out");
int a[1000][1000];

int main() {
    fill_n(&a[0][0], sizeof(a)/sizeof(int), 1);
    int n,num,x,y;
    fin>>n>>num;
    for(int i=0;i<num;i++) {
	fin>>x>>y;
	a[x-1][y-1]=0;
    }
    int res=0;
    for(int i=0;i<n;i++){
	for(int j=0;j<n;j++) {
	    if(a[i][j]) {
		if(i!=0&&j!=0)
		    a[i][j]=min( min(a[i-1][j],a[i][j-1]),a[i-1][j-1])+1;
		if(res<a[i][j])
		    res=a[i][j];
	    }
	}
    }
    fout<<res<<endl;
    return 0;
}
