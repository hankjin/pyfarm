/*
ID: hankjin1
LANG: C++
TASK: humble
*/
#include <iostream>
#include <fstream>
#include <cmath>
#include <climits>
#include <set>
#include <algorithm>
using namespace std;
//note: delete used nums in set
//note: notice overflow of multiply

int K,N;
int found;
long long prime[100];//supplied prime
int pcount[100];//prime multied num index in humble
long long humble[100002];//humble his
int humbleN;//humble count

int main(){
    ifstream fin("humble.in");
    ofstream fout("humble.out");
    fin>>K>>N;
    for(int i=0;i<K;i++){
	fin>>prime[i];
    }
    long long res,tmp;
    humble[humbleN++]=1;
    for(int i=0;i<N;i++){
	res=INT_MAX;
	for(int j=0;j<K;j++){
	    while(prime[j]*humble[pcount[j]] <= humble[i])
		pcount[j]++;
	    if(prime[j]*humble[pcount[j]]<res)
		res = prime[j]*humble[pcount[j]];
	}
//	cout<<res<<endl;
	humble[humbleN++]=res;
//	cout<<res<<","<<nums.size()<<endl;
    }
    fout<<humble[N]<<endl;
    return 0;
}
