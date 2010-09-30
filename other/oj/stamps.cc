/*
ID: hankjin1
LANG: C++
TASK: stamps
*/
#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

//note: bag problem
//dp: d[i]=min(d[i-stamp[1~n]])

int K,N;
int res[2000002];
int stamps[200];
int main(){
    ifstream fin("stamps.in");
    ofstream fout("stamps.out");
    fin>>K>>N;
    fill_n(&res[0], 2000002, K+1);
    for(int i=0;i<N;i++){
	fin>>stamps[i];
	res[stamps[i]]=1;
    }
    int i=2;
    for(;;i++){//2=1+1, 3=1+2
	for(int j=0;j<N;j++){
	    if(i>stamps[j] && res[i]>res[i-stamps[j]]+1)
		res[i]=res[i-stamps[j]]+1;
	}
	if(res[i]>K)
	    break;
    }
    fout<<(i-1)<<endl;

    return 0;
}
