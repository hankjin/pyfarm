/*
ID: hankjin1
LANG: C++
TASK: holstein
*/
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <vector>
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

int total[25];
vector<int> found;

int main(){
    int i,j,k;
    fin>>V;
    for(i=0;i<V;i++){
	fin>>vs[i];
    }
    fin>>G;
    for(i=0;i<G;i++){
	for(j=0;j<V;j++){
	    fin>>gs[i][j];
	}
    }
    for(i=1;i<pow(2,G);i++){
	memset(total, 0, sizeof(int)*25);
//	fill_n(total, 0, 25);
	for(j=0;j<G;j++){//
	    if( ((i>>j)&1)>0){
		for(k=0;k<V;k++){
		    total[k]+=gs[j][k];
		}
	    }
	}
	for(j=0;j<V;j++){
	    if(total[j]<vs[j]){
		break;
	    }
	}
	if(j==V)
	    found.push_back(i);
    }
    for(vector<int>::iterator it=found.begin();it!=found.end();it++){
	cout<<*it<<",";
    }
    cout<<endl;
    return 0;
}
