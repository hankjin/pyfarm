/**
ID: hankjin1
LANG: C++
TASK: concom
*/
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
using namespace std;
//note: dp, if a owns b c, first update those own a, then if c>50, then xx

ifstream fin("concom.in");
ofstream fout("concom.out");

int N;
int maxN;
int nums[101][101];

void addown(int a, int b){
    for(int i=1;i<=maxN;i++){
	if(nums[i][a]>50){
	    for(int j=1;j<=maxN;j++){
		nums[i][j]+=nums[b][j];
	    }
	}
    }
}
int main(){
    fin>>N;
    int a,b,v;
    for(int i=0;i<N;i++){
	fin>>a>>b>>v;
	maxN = max(maxN, max(a,b));
	nums[a][b]=v;
    }
    for(int a=1;a<=maxN;a++){
	for(int b=1;b<=maxN;b++){
	    if(nums[a][b]==0)
		continue;
	    for(int j=1;j<=maxN;j++){//a owned, then b owned
		if(nums[j][a]>50)
		    nums[j][b]+=nums[a][b];
	    }
	    if(nums[a][b]>50)//b owned, then those b owned owned
		addown(a,b);
	}
    }
    for(int i=1;i<=maxN;i++){
	for(int j=1;j<=maxN;j++){
	    if(nums[i][j]>50)
		fout<<i<<" "<<j<<endl;
	}
    }
    return 0;
}
